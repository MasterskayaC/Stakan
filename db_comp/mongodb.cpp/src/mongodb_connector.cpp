#include "mongodb_connector.hpp"


#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace ob {
namespace {
mongocxx::instance& mongo_instance() {
    static mongocxx::instance inst{};
    return inst;
}

}  // namespace

class MongoDbConnector::Impl {
public:
    explicit Impl(Config c) : cfg_(std::move(c)) {}

    // Копия настроек: URI, имя БД, имена коллекций.
    Config cfg_;
    // Клиент держит пул соединений к серверу; создаётся один раз и переиспользуется.
    std::unique_ptr<mongocxx::client> client_;
    mongocxx::database db_;

    // Создаём клиента при первом запросе, проверяет доступность сервера командой пинг.
    // При ошибке сети/авторизации клиент сбрасывается, чтобы следующий вызов попробовал снова.
    bool ensure_client(std::string* err) {
        (void)mongo_instance();
        try {
            if (!client_) {
                // разбираем строку подключения (хосты, опции).
                client_ = std::make_unique<mongocxx::client>(mongocxx::uri{cfg_.uri});
                db_ = (*client_)[cfg_.db];
                // пинг минимально нагружает сервер и гарантирует, что топология доступна.
                db_.run_command(bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("ping", 1)));
            }
            return true;
        } catch (const std::exception& e) {
            if (err) {
                *err = e.what();
            }
            client_.reset();
            return false;
        }
    }
};

MongoDbConnector::MongoDbConnector(Config cfg)
    : impl_(std::make_unique<Impl>(std::move(cfg))) {}

MongoDbConnector::~MongoDbConnector() = default;

MongoDbConnector::MongoDbConnector(MongoDbConnector&&) noexcept = default;
MongoDbConnector& MongoDbConnector::operator=(MongoDbConnector&&) noexcept = default;

bool MongoDbConnector::connect(std::string* error_out) {
    return impl_->ensure_client(error_out);
}

void MongoDbConnector::disconnect() {
    // Закрываем соединения; следующий connect/insert снова пройдёт через ensure_client.
    impl_->client_.reset();
}

bool MongoDbConnector::insert_snapshot(const BookSnapshotDoc& snap, std::string* error_out) {
    if (!impl_->ensure_client(error_out)) {
        return false;
    }
    using bsoncxx::builder::basic::kvp;

    // Собираем BSON-документ поле за полем — в БД документ появится с теми же ключами.
    bsoncxx::builder::basic::document body;
    body.append(kvp("seq", snap.seq));
    body.append(kvp("ts_ms", snap.ts_ms));
    body.append(kvp("bid_price", snap.bid.price));
    body.append(kvp("bid_qty", snap.bid.qty));
    body.append(kvp("ask_price", snap.ask.price));
    body.append(kvp("ask_qty", snap.ask.qty));
    if (!snap.payload.empty()) {
        // Произвольный бинарный хвост (например, сериализация готового стакана).
        body.append(kvp(
            "payload",
            bsoncxx::types::b_binary{
                bsoncxx::binary_sub_type::k_binary, static_cast<std::uint32_t>(snap.payload.size()),
                snap.payload.data()}));
    }

    try {
        // возвращаем коллекцию; при первой вставке она создаётся на сервере.
        auto coll = impl_->db_[impl_->cfg_.coll_snapshots];
        auto res = coll.insert_one(body.view());
        return static_cast<bool>(res);
    } catch (const std::exception& e) {
        if (error_out) {
            *error_out = e.what();
        }
        return false;
    }
}

bool MongoDbConnector::insert_trade(const TradeDoc& t, std::string* error_out) {
    if (!impl_->ensure_client(error_out)) {
        return false;
    }
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    // Один документ = одно событие в журнале
    auto doc = make_document(
        kvp("ts_ms", t.ts_ms),
        kvp("order_id", static_cast<std::int64_t>(t.order_id)),
        kvp("price", t.price),
        kvp("qty", t.qty),
        kvp("side", std::string(1, t.side)));

    try {
        auto coll = impl_->db_[impl_->cfg_.coll_trades];
        auto res = coll.insert_one(doc.view());
        return static_cast<bool>(res);
    } catch (const std::exception& e) {
        if (error_out) {
            *error_out = e.what();
        }
        return false;
    }
}

bool MongoDbConnector::insert_stats_json(std::int64_t ts_ms, const std::string& json_body,
                                         std::string* error_out) {
    if (!impl_->ensure_client(error_out)) {
        return false;
    }
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    try {
        //строимBSON из текста
        auto embedded = bsoncxx::from_json(json_body);
        bsoncxx::builder::basic::document wrapper;
        wrapper.append(kvp("ts_ms", ts_ms));
        wrapper.append(kvp("data", embedded));

        auto coll = impl_->db_[impl_->cfg_.coll_stats];
        auto res = coll.insert_one(wrapper.view());
        return static_cast<bool>(res);
    } catch (const std::exception& e) {
        if (error_out) {
            *error_out = e.what();
        }
        return false;
    }
}

std::optional<BookSnapshotDoc> MongoDbConnector::load_latest_snapshot(std::string* error_out) {
    if (!impl_->ensure_client(error_out)) {
        return std::nullopt;
    }

    try {
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        mongocxx::options::find opts;
        // Сортировка по убыванию
        opts.sort(make_document(kvp("seq", -1)));
        opts.limit(1);

        auto coll = impl_->db_[impl_->cfg_.coll_snapshots];
        auto cursor = coll.find({}, opts);
        auto it = cursor.begin();
        if (it == cursor.end()) {
            return std::nullopt;
        }
        auto v = *it;
        // бросает если тип в документе другой 
        BookSnapshotDoc out{};
        if (auto el = v["seq"]) {
            out.seq = el.get_int64().value;
        }
        if (auto el = v["ts_ms"]) {
            out.ts_ms = el.get_int64().value;
        }
        if (auto el = v["bid_price"]) {
            out.bid.price = static_cast<std::int32_t>(el.get_int32().value);
        }
        if (auto el = v["bid_qty"]) {
            out.bid.qty = static_cast<std::int32_t>(el.get_int32().value);
        }
        if (auto el = v["ask_price"]) {
            out.ask.price = static_cast<std::int32_t>(el.get_int32().value);
        }
        if (auto el = v["ask_qty"]) {
            out.ask.qty = static_cast<std::int32_t>(el.get_int32().value);
        }
        if (auto el = v["payload"]) {
            auto bin = el.get_binary();
            out.payload.assign(bin.bytes, bin.bytes + bin.size);
        }
        return out;
    } catch (const std::exception& e) {
        if (error_out) {
            *error_out = e.what();
        }
        return std::nullopt;
    }
}

}  // namespace ob
