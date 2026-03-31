#include "bid_ask_book.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <format>
#include <mutex>

using namespace server;
using namespace common;

namespace {
bool validate(const Order& order, const char* operation) {
    if (order.price == 0 || order.quantity == 0) {
        Logger::Log(LogLevel::Error,
            std::format("{}: Invalid order, id = {}", operation, order.id));
        return false;
    }

    return true;
}
}

void Logger::Log(const LogLevel level, const std::string &msg) {
    std::cout << "[" << Now() << "] "
            << "[" << ToString(level) << "] "
            << msg << std::endl;
}

std::string Logger::Now() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

const char * Logger::ToString(const LogLevel level) {
    switch (level) {
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
    }
    return "";
}

void OrderBook::NewBid(Order order) {
    if (!validate(order, "NewBid")) {
        return;
    }

    std::unique_lock lock(bids_mutex_);

    auto result = bids_.insert(order);
    if (result.second) {
        Logger::Log(LogLevel::Info,
            std::format("New BID added, id = {}", order.id));
    } else {
        Logger::Log(LogLevel::Error,
            std::format("NewBid: BID id = {} already exists", order.id));
    }
}

void OrderBook::NewAsk(Order order) {
    if (!validate(order, "NewAsk")) {
        return;
    }

    std::unique_lock lock(asks_mutex_);

    auto result = asks_.insert(order);
    if (result.second) {
        Logger::Log(LogLevel::Info,
            std::format("New ASK added, id = {}", order.id));
    } else {
        Logger::Log(LogLevel::Error,
            std::format("NewAsk: ASK id = {} already exists", order.id));
    }
}

void OrderBook::CancelBid(ID order_id) {
    std::unique_lock lock(bids_mutex_);

    auto& index = bids_.get<1>();
    auto it = index.find(order_id);
    if (it != index.end()) {
        index.erase(it);
        Logger::Log(LogLevel::Info,
            std::format("Bid canceled, id = {}", order_id));
    } else {
        Logger::Log(LogLevel::Error,
            std::format("CancelBid: Invalid id = {}", order_id));
    }
}

void OrderBook::CancelAsk(ID order_id) {
    std::unique_lock lock(asks_mutex_);

    auto& index = asks_.get<1>();
    auto it = index.find(order_id);
    if (it != index.end()) {
        index.erase(it);
        Logger::Log(LogLevel::Info,
            std::format("Ask canceled, id = {}", order_id));
    } else {
        Logger::Log(LogLevel::Error,
            std::format("CancelAsk: Invalid id = {}", order_id));
    }
}

void OrderBook::ReplaceBid(Order old_order, Order new_order) {
    if (new_order.price == 0 || new_order.quantity == 0) {
        Logger::Log(LogLevel::Error,
            std::format("ReplaceBid: Invalid order, id = {}", new_order.id));
        return;
    }
    if (old_order.id != new_order.id) {
        Logger::Log(LogLevel::Error,
        std::format("ReplaceBid: Order id cannot be changed (old id = {}, new id = {})",
                    old_order.id, new_order.id));
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
        Logger::Log(LogLevel::Info,
            std::format("Replaced BID id = {}", new_order.id));
    } else {
        Logger::Log(LogLevel::Error,
            std::format("ReplaceBid:, id {} not found", old_order.id));
    }
}

void OrderBook::ReplaceAsk(Order old_order, Order new_order) {
    if (new_order.price == 0 || new_order.quantity == 0) {
        Logger::Log(LogLevel::Error,
            std::format("ReplaceAsk: Invalid order, id = {}", new_order.id));
        return;
    }
    if (old_order.id != new_order.id) {
        Logger::Log(LogLevel::Error,
        std::format("ReplaceAsk: Order id cannot be changed (old id = {}, new id = {})",
                    old_order.id, new_order.id));
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
        Logger::Log(LogLevel::Info,
            std::format("Replaced ASK id = {}", new_order.id));
    } else {
        Logger::Log(LogLevel::Error,
            std::format("ReplaceAsk:, id {} not found", old_order.id));
    }
}

Snapshot OrderBook::GetTopSnapshot() const {
    Snapshot snapshot{};

    std::scoped_lock lock(bids_mutex_, asks_mutex_);

    const auto fill_top_orders = [](const auto& index, auto& target, const char* side) {
        if (index.empty()) {
            Logger::Log(LogLevel::Warning, std::format("GetTopSnapshot: No {} available", side));
        } else if (index.size() < topN) {
            Logger::Log(LogLevel::Warning,
                std::format("GetTopSnapshot: not enough {}, {} available", side, index.size()));
        }

        size_t count = 0;
        for (const auto& order : index) {
            if (count >= topN) break;
            target[count++] = order;
        }
    };

    fill_top_orders(bids_.get<0>(), snapshot.topBids, "Bids");
    fill_top_orders(asks_.get<0>(), snapshot.topAsks, "Asks");

    return snapshot;
}

Order OrderBook::BestBid() const {
    std::shared_lock lock(bids_mutex_);

    if (bids_.empty()) {
        Logger::Log(LogLevel::Warning, "BestBid: bids are empty");
        return {0,0,0};
    }
    const auto& bid_index = bids_.get<0>();
    return *bid_index.begin();
}

Order OrderBook::BestAsk() const {
    std::shared_lock lock(asks_mutex_);

    if (asks_.empty()) {
        Logger::Log(LogLevel::Warning, "BestAsk: asks are empty");
        return {0,0,0};
    }
    const auto& ask_index = asks_.get<0>();
    return *ask_index.begin();
}
