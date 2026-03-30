#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ob {

// Уровень цены/объёма для одной стороны книги (бид или аск).
// Должен соответствовать вашим типам в маркет-дате / снимке стакана.
struct BestLevel {
    std::int32_t price{};
    std::int32_t qty{};
};

// Документ «снимка» лучших уровней для записи в MongoDB.
struct BookSnapshotDoc {
    std::int64_t seq{};    // Монотонный номер события от движка (для порядка и выборки «последнего»)
    std::int64_t ts_ms{};  // Время в миллисекундах
    BestLevel bid{};
    BestLevel ask{};
    // Необязательно: бинар от модуля сериализации (глубина стакана, расширенный snapshot).
    std::vector<std::uint8_t> payload;
};

// Одна запись о сделке/событии для журнала в коллекции trades.
struct TradeDoc {
    std::int64_t ts_ms{};
    std::int64_t order_id{};
    std::int32_t price{};
    std::int32_t qty{};
    char side{};  // 'B' — покупка, 'S' — продажа 
};

// Коннектор к MongoDB
class MongoDbConnector {
public:
    struct Config {
        std::string uri =
            "mongodb://127.0.0.1:27017/?appName=orderbook"; //  локальный сервер MongoDB
        std::string db = "orderbook"; // имя базы данных
        std::string coll_snapshots = "snapshots"; // коллекция для снимков лучшего бид/аск
        std::string coll_trades = "trades"; // коллекция для журнала сделок
        std::string coll_stats = "stats"; // коллекция для статистики, куда кладёт 
    };

    explicit MongoDbConnector(Config cfg = {});
    ~MongoDbConnector();

    MongoDbConnector(const MongoDbConnector&) = delete;
    MongoDbConnector& operator=(const MongoDbConnector&) = delete;
    MongoDbConnector(MongoDbConnector&&) noexcept;
    MongoDbConnector& operator=(MongoDbConnector&&) noexcept;

    // Подключение к серверу и проверка связи (ping). false — ошибка; текст можно взять из error_out.
    bool connect(std::string* error_out = nullptr);

    void disconnect();

    // Вставить один документ снимка в коллекцию snapshots.
    bool insert_snapshot(const BookSnapshotDoc& snap, std::string* error_out = nullptr);

    // Вставить запись о сделке в коллекцию trades.
    bool insert_trade(const TradeDoc& t, std::string* error_out = nullptr);

    // Сохранить статистику: ts_ms + вложенный JSON.
    bool insert_stats_json(std::int64_t ts_ms, const std::string& json_body,
                           std::string* error_out = nullptr);

    // Прочитать последний снимок по полю seq (для холодного старта клиента / POC восстановления).
    std::optional<BookSnapshotDoc> load_latest_snapshot(std::string* error_out = nullptr);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace ob
