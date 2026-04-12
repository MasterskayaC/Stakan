#include "bid_ask_book.h"

#include <format>
#include <mutex>

using namespace server;
using namespace common;

namespace {
bool validate(const Order& order, const char* operation) {
    if (order.price == 0 || order.quantity == 0) {
        Logger::Log(LogLevel::Error, std::format("{}: Invalid order, id = {}", operation, order.id));
        return false;
    }

    return true;
}
}  // namespace

// Index, так как работаем с двумя контейнерами: BidContainer и AskContainer
template <typename Index>
bool OrderBook::IsInTopN(const Index& index, const ID order_id, const size_t top_n) {
    size_t count = 0;
    for (const auto& order : index) {
        if (count >= top_n) {
            break;
        }
        if (order.id == order_id) {
            return true;
        }
        ++count;
    }

    return false;
}

void OrderBook::AllowBuildNewSnapshot() {
    is_ready_new_snapshot_.store(true, std::memory_order_release);
}

void OrderBook::NewBid(Order order) {
    if (!validate(order, "NewBid")) {
        return;
    }

    std::unique_lock lock(bids_mutex_);

    auto result = bids_.insert(order);
    if (result.second) {
        if (IsInTopN(bids_.get<0>(), order.id, topN)) {
            AllowBuildNewSnapshot();
        }
        md_updates_.emplace(GenerateMDUpdate(order, MDUpdate::UpdateType::Add, true));
        Logger::Log(LogLevel::Info, std::format("New BID added, id = {}", order.id));
    } else {
        Logger::Log(LogLevel::Error, std::format("NewBid: BID id = {} already exists", order.id));
    }    
}

void OrderBook::NewAsk(Order order) {
    if (!validate(order, "NewAsk")) {
        return;
    }

    std::unique_lock lock(asks_mutex_);

    auto result = asks_.insert(order);
    if (result.second) {
        if (IsInTopN(asks_.get<0>(), order.id, topN)) {
            AllowBuildNewSnapshot();
        }
        md_updates_.emplace(GenerateMDUpdate(order, MDUpdate::UpdateType::Add, false));
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
        const bool was_in_top = IsInTopN(bids_.get<0>(), order_id, topN);
        index.erase(it);
        if (was_in_top) {
            AllowBuildNewSnapshot();
        }
        md_updates_.emplace(GenerateMDUpdate(*it, MDUpdate::UpdateType::Delete, true));
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
        const bool was_in_top = IsInTopN(asks_.get<0>(), order_id, topN);
        index.erase(it);
        if (was_in_top) {
            AllowBuildNewSnapshot();
        }
        md_updates_.emplace(GenerateMDUpdate(*it, MDUpdate::UpdateType::Delete, false));
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
        const bool was_in_top = IsInTopN(bids_.get<0>(), old_order.id, topN);
        index.modify(it, [&](Order& o) {
            o.price = new_order.price;
            o.quantity = new_order.quantity;
        });
        if (was_in_top || IsInTopN(bids_.get<0>(), new_order.id, topN)) {
            AllowBuildNewSnapshot();
        }
        md_updates_.emplace(GenerateMDUpdate(new_order, MDUpdate::UpdateType::Modify, true));
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
        const bool was_in_top = IsInTopN(asks_.get<0>(), old_order.id, topN);
        index.modify(it, [&](Order& o) {
            o.price = new_order.price;
            o.quantity = new_order.quantity;
        });
        if (was_in_top || IsInTopN(asks_.get<0>(), new_order.id, topN)) {
            AllowBuildNewSnapshot();
        }
        md_updates_.emplace(GenerateMDUpdate(new_order, MDUpdate::UpdateType::Modify, false));
        Logger::Log(LogLevel::Info, std::format("Replaced ASK id = {}", new_order.id));
    } else {
        Logger::Log(LogLevel::Error, std::format("ReplaceAsk:, id {} not found", old_order.id));
    }    
}

Snapshot OrderBook::BuildNewSnapshot() const {
    Snapshot snapshot{};

    const auto fill_top_orders = [](const auto& index, auto& target, const char* side) {
        if (index.empty()) {
            Logger::Log(LogLevel::Warning, std::format("GetTopSnapshot: No {} available", side));
        } else if (index.size() < topN) {
            Logger::Log(LogLevel::Warning,
                        std::format("GetTopSnapshot: not enough {}, {} available", side, index.size()));
        }

        size_t count = 0;
        for (const auto& order : index) {
            if (count >= topN)
                break;
            target[count++] = order;
        }
    };

    fill_top_orders(bids_.get<0>(), snapshot.topBids, "Bids");
    fill_top_orders(asks_.get<0>(), snapshot.topAsks, "Asks");

    return snapshot;
}

std::optional<Snapshot> OrderBook::GetTopSnapshot() const {
    std::scoped_lock order_lock(bids_mutex_, asks_mutex_);

    if (is_ready_new_snapshot_.exchange(false, std::memory_order_acq_rel)) {
        return BuildNewSnapshot();
    }

    return std::nullopt;
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

MDUpdate OrderBook::GenerateMDUpdate(const common::Order& order, MDUpdate::UpdateType type, bool is_bid) const {
    return {type, is_bid, order};
}

std::optional<MDUpdate> OrderBook::GetCurrentMDUpdate() {
    if (md_updates_.empty()) {
        Logger::Log(LogLevel::Warning, "No current MDUpdate");
        return std::nullopt;
    }
    std::optional<MDUpdate> curr = md_updates_.front();
    md_updates_.pop();
    return curr;
}
