#include "bid_ask_book.h"
#include <format>
#include <mutex>

using namespace server;
using namespace common;

namespace {
bool Validate(const Order& order, const char* operation) {
    if (order.price == 0 || order.quantity == 0) {
        Logger::Log(LogLevel::Error, std::format("{}: Invalid order, id = {}", operation, order.id));
        return false;
    }

    return true;
}
}  // namespace

void OrderBook::NewBid(Order order) {
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

void OrderBook::NewAsk(Order order) {
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

void OrderBook::CancelBid(ID order_id) {
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

void OrderBook::CancelAsk(ID order_id) {
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

void OrderBook::ReplaceBid(Order old_order, Order new_order) {
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

void OrderBook::ReplaceAsk(Order old_order, Order new_order) {
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

Snapshot OrderBook::GetTopSnapshot() const {
    // Snapshot snapshot{};
    //
    // std::scoped_lock lock(bids_mutex_, asks_mutex_);
    //
    // const auto& bid_index = bids_.get<0>();
    // if (bid_index.empty()) {
    //     Logger::Log(LogLevel::Warning, "GetTopSnapshot: No Bids available");
    // } else if (bid_index.size() < topN) {
    //     Logger::Log(LogLevel::Warning,
    //         std::format("GetTopSnapshot: not enough Bids, {} available", bid_index.size()));
    // }
    // size_t count = 0;
    // for (const auto& bid : bid_index) {
    //     if (count >= topN) break;
    //     snapshot.topBids[count++] = bid;
    // }
    //
    // const auto& ask_index = asks_.get<0>();
    // if (ask_index.empty()) {
    //     Logger::Log(LogLevel::Warning, "GetTopSnapshot: No Asks available");
    // } else if (ask_index.size() < topN) {
    //     Logger::Log(LogLevel::Warning,
    //         std::format("GetTopSnapshot: not enough Asks, {} available", ask_index.size()));
    // }
    // count = 0;
    // for (const auto& ask : ask_index) {
    //     if (count >= topN) break;
    //     snapshot.topAsks[count++] = ask;
    // }
    //
    // return snapshot;
    return *snapshot_source_->get_snapshot();
}

Order OrderBook::BestBid() const {
    std::shared_lock lock(bids_mutex_);

    if (bids_.empty()) {
        Logger::Log(LogLevel::Warning, "BestBid: bids are empty");
        return {0, 0, 0};
    }
    const auto& bid_index = bids_.get<0>();
    return *bid_index.begin();
}

Order OrderBook::BestAsk() const {
    std::shared_lock lock(asks_mutex_);

    if (asks_.empty()) {
        Logger::Log(LogLevel::Warning, "BestAsk: asks are empty");
        return {0, 0, 0};
    }
    const auto& ask_index = asks_.get<0>();
    return *ask_index.begin();
}

/**
 * @brief шаблонная функция для поиска по цене
 * @param container контейнер bids_ или asks_
 * @param price цена
 * @param message текст ошибки
 * @return объект PricesInfo с контейнером IDs объект по этой цене и количество этих объектов
 */
template <typename Container>
PricesInfo GetContainerPrice(const Container& container, common::Price price, std::string message) {
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
 * @brief реализация функции поиска бидов по цене для класса OrderBook
 * @param price цена
 * @return объект PricesInfo с контейнером IDs объект по этой цене и количество этих объектов
 */
PricesInfo OrderBook::GetPricesBidsInfo(common::Price price) const {
    std::shared_lock lock(bids_mutex_);
    return GetContainerPrice<BidContainer>(bids_, price, "Not bids for this price");
}

/**
 * @brief реализация функции поиска асков по цене для класса OrderBook
 * @param price цена
 * @return объект PricesInfo с контейнером IDs объект по этой цене и количество этих объектов
 */
PricesInfo OrderBook::GetPricesAsksInfo(common::Price price) const {
    std::shared_lock lock(asks_mutex_);
    return GetContainerPrice<AskContainer>(asks_, price, "Not asks for this price");
}
