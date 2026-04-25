#include "bid_ask_book.h"
#include <format>
#include <mutex>

using namespace server;
using namespace common;
namespace {
constexpr auto kSnapshotFeedInterval = std::chrono::milliseconds(50);

bool Validate(const Order& order, const char* operation) {
    if (order.price == 0 || order.quantity == 0) {
        Logger::Log(LogLevel::Error, std::format("{}: Invalid order, id = {}", operation, order.id));
        return false;
    }

    return true;
}
}  // namespace

OrderBook::OrderBook(bool enable_snapshot_feed) : snapshot_feed_enabled_(enable_snapshot_feed) {
    if (snapshot_feed_enabled_) {
        StartSnapshotFeed();
    }
}

OrderBook::~OrderBook() {
    StopSnapshotFeed();
}

void OrderBook::StartSnapshotFeed() {
    if (snapshot_feed_running_.exchange(true)) {
        return;
    }

    snapshot_feed_thread_ = std::thread(&OrderBook::SnapshotFeedLoop, this);
}

void OrderBook::StopSnapshotFeed() {
    if (!snapshot_feed_running_.exchange(false)) {
        return;
    }

    snapshot_feed_cv_.notify_all();
    if (snapshot_feed_thread_.joinable()) {
        snapshot_feed_thread_.join();
    }
}

void OrderBook::SnapshotFeedLoop() {
    while (snapshot_feed_running_) {
        NewBid(snapshot_source_->GetNewBid());
        NewAsk(snapshot_source_->GetNewAsk());

        std::unique_lock lock(snapshot_feed_mutex_);
        snapshot_feed_cv_.wait_for(lock, kSnapshotFeedInterval, [this] {
            return !snapshot_feed_running_.load();
        });
    }
}

inline void OrderBook::NewBid(Order order) {
    if (!Validate(order, "NewBid")) {
        return;
    }

    std::unique_lock lock(bids_mutex_);

    auto result = bids_.insert(order);
    if (result.second) {
        Logger::Log(LogLevel::Info, std::format("New BID added, id = {}", order.id));
    } else {
        Logger::Log(LogLevel::Error, std::format("NewBid: BID id = {} already exists", order.id));
    }
}

inline void OrderBook::NewAsk(Order order) {
    if (!Validate(order, "NewAsk")) {
        return;
    }

    std::unique_lock lock(asks_mutex_);

    auto result = asks_.insert(order);
    if (result.second) {
        Logger::Log(LogLevel::Info, std::format("New ASK added, id = {}", order.id));
    } else {
        Logger::Log(LogLevel::Error, std::format("NewAsk: ASK id = {} already exists", order.id));
    }
}

inline void OrderBook::CancelBid(ID order_id) {
    std::unique_lock lock(bids_mutex_);

    auto& index = bids_.get<1>();
    auto it = index.find(order_id);
    if (it != index.end()) {
        index.erase(it);
        Logger::Log(LogLevel::Info, std::format("Bid canceled, id = {}", order_id));
    } else {
        Logger::Log(LogLevel::Error, std::format("CancelBid: Invalid id = {}", order_id));
    }
}

inline void OrderBook::CancelAsk(ID order_id) {
    std::unique_lock lock(asks_mutex_);

    auto& index = asks_.get<1>();
    auto it = index.find(order_id);
    if (it != index.end()) {
        index.erase(it);
        Logger::Log(LogLevel::Info, std::format("Ask canceled, id = {}", order_id));
    } else {
        Logger::Log(LogLevel::Error, std::format("CancelAsk: Invalid id = {}", order_id));
    }
}

inline void OrderBook::ReplaceBid(Order old_order, Order new_order) {
    if (new_order.price == 0 || new_order.quantity == 0) {
        Logger::Log(LogLevel::Error, std::format("ReplaceBid: Invalid order, id = {}", new_order.id));
        return;
    }
    if (old_order.id != new_order.id) {
        Logger::Log(LogLevel::Error,
                    std::format("ReplaceBid: Order id cannot be changed (old id = {}, new id = {})",
                                old_order.id,
                                new_order.id));
        return;
    }

    std::unique_lock lock(bids_mutex_);

    auto& index = bids_.get<1>();
    auto it = index.find(old_order.id);
    if (it != index.end()) {
        index.modify(it, [&](Order& o) {
            o.price = new_order.price;
            o.quantity = new_order.quantity;
        });
        Logger::Log(LogLevel::Info, std::format("Replaced BID id = {}", new_order.id));
    } else {
        Logger::Log(LogLevel::Error, std::format("ReplaceBid:, id {} not found", old_order.id));
    }
}

inline void OrderBook::ReplaceAsk(Order old_order, Order new_order) {
    if (new_order.price == 0 || new_order.quantity == 0) {
        Logger::Log(LogLevel::Error, std::format("ReplaceAsk: Invalid order, id = {}", new_order.id));
        return;
    }
    if (old_order.id != new_order.id) {
        Logger::Log(LogLevel::Error,
                    std::format("ReplaceAsk: Order id cannot be changed (old id = {}, new id = {})",
                                old_order.id,
                                new_order.id));
        return;
    }

    std::unique_lock lock(asks_mutex_);

    auto& index = asks_.get<1>();
    auto it = index.find(old_order.id);
    if (it != index.end()) {
        index.modify(it, [&](Order& o) {
            o.price = new_order.price;
            o.quantity = new_order.quantity;
        });
        Logger::Log(LogLevel::Info, std::format("Replaced ASK id = {}", new_order.id));
    } else {
        Logger::Log(LogLevel::Error, std::format("ReplaceAsk:, id {} not found", old_order.id));
    }
}

inline Snapshot OrderBook::GetTopSnapshot() const {
    Snapshot snapshot{};
    std::shared_lock bids_lock(bids_mutex_);
    std::shared_lock asks_lock(asks_mutex_);

    const auto& bid_index = bids_.get<0>();
    size_t count = 0;
    for (const auto& bid : bid_index) {
        if (count >= topN) {
            break;
        }
        snapshot.topBids[count++] = bid;
    }

    const auto& ask_index = asks_.get<0>();
    count = 0;
    for (const auto& ask : ask_index) {
        if (count >= topN) {
            break;
        }
        snapshot.topAsks[count++] = ask;
    }

    return snapshot;
}

inline Order OrderBook::BestBid() const {
    std::shared_lock lock(bids_mutex_);

    if (bids_.empty()) {
        Logger::Log(LogLevel::Warning, "BestBid: bids are empty");
        return {0, 0, 0};
    }
    const auto& bid_index = bids_.get<0>();
    return *bid_index.begin();
}

inline Order OrderBook::BestAsk() const {
    std::shared_lock lock(asks_mutex_);

    if (asks_.empty()) {
        Logger::Log(LogLevel::Warning, "BestAsk: asks are empty");
        return {0, 0, 0};
    }
    const auto& ask_index = asks_.get<0>();
    return *ask_index.begin();
}

inline std::optional<common::MDUpdate> OrderBook::GenerateMDUpdate() const {
    common::Order kEmptyOrder(0, 0, 0);

    Order best_bid = BestBid();
    Order best_ask = BestAsk();

    if (best_bid == kEmptyOrder && best_ask == kEmptyOrder) {
        Logger::Log(LogLevel::Warning, "OrderBook: is empty");
        return std::nullopt;
    }

    PricesInfo best_bid_price_info = GetPricesBidsInfo(best_bid.price);
    PricesInfo best_ask_price_info = GetPricesAsksInfo(best_ask.price);

    return common::MDUpdate{.best_bid_price = best_bid.price,
                            .best_bid_qty = best_bid_price_info.quantity_,
                            .best_ask_price = best_ask.price,
                            .best_ask_qty = best_ask_price_info.quantity_};
}

/**
 * @brief Шаблонная функция для агрегации заявок по уровню цены.
 * @param container Контейнер bids_ или asks_.
 * @param price Цена, по которой выполняется поиск.
 * @param message Текст предупреждения для случая пустого контейнера.
 * @return Структура с id найденных заявок и их суммарным количеством.
 */
template <typename Container>
inline PricesInfo GetContainerPrice(const Container& container, common::Price price, std::string message) {
    if (container.empty()) {
        Logger::Log(LogLevel::Warning, message);
        return {};
    }
    PricesInfo info;
    const auto& index = container.template get<2>();
    auto range = index.equal_range(price);
    for (auto it = range.first; it != range.second; it++) {
        info.ids_.push_back(it->id);
        info.quantity_ += it->quantity;
    }
    return info;
}

/**
 * @brief Возвращает агрегированную информацию по bid-заявкам на заданной цене.
 * @param price Цена, по которой выполняется поиск.
 * @return Структура с id найденных bid-заявок и их суммарным количеством.
 */
inline PricesInfo OrderBook::GetPricesBidsInfo(common::Price price) const {
    std::shared_lock lock(bids_mutex_);
    return GetContainerPrice<BidContainer>(bids_, price, "Not bids for this price");
}

/**
 * @brief Возвращает агрегированную информацию по ask-заявкам на заданной цене.
 * @param price Цена, по которой выполняется поиск.
 * @return Структура с id найденных ask-заявок и их суммарным количеством.
 */
inline PricesInfo OrderBook::GetPricesAsksInfo(common::Price price) const {
    std::shared_lock lock(asks_mutex_);
    return GetContainerPrice<AskContainer>(asks_, price, "Not asks for this price");
}
