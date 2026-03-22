#include "bid_ask_book.h"
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace server;

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

std::string server::SideToString(Side side) {
    switch (side) {
        case Side::Bid: return "BID";
        case Side::Ask: return "ASK";
    }
    return "";
}

void OrderBook::NewOrder(const Order& order) {
    if (order.price == 0 || order.quantity == 0) {
        Logger::Log(LogLevel::Error, "NewOrder: Invalid order, id = "
            + std::to_string(order.id));
        return;
    }

    switch (order.side) {
        case Side::Ask : {
            asks_.insert(order);
            Logger::Log(LogLevel::Info, "New ask added, id = "
                + std::to_string(order.id));
        } break;

        case Side::Bid : {
            bids_.insert(order);
            Logger::Log(LogLevel::Info, "New bid added, id = "
                + std::to_string(order.id));

        } break;

        default: Logger::Log(LogLevel::Error, "NewOrder: Unexpected order side, id = "
            + std::to_string(order.id));
    }
}

void OrderBook::CancelOrder(const Side side, const ID order_id) {
    auto cancel_in_index = [&](auto& index, const Side s) {
        auto it = index.find(order_id);
        if (it != index.end()) {
            index.erase(it);
            Logger::Log(LogLevel::Info, "Order canceled: " + server::SideToString(s)
                        + ", id = " + std::to_string(order_id));
        } else {
            Logger::Log(LogLevel::Error, "CancelOrder: Invalid id = " + std::to_string(order_id));
        }
    };

    switch (side) {
        case Side::Ask: {
            auto& ask_index = asks_.get<1>();
            cancel_in_index(ask_index, side);
        } break;

        case Side::Bid: {
            auto& bid_index = bids_.get<1>();
            cancel_in_index(bid_index, side);
        } break;

        default:
            Logger::Log(LogLevel::Error, "CancelOrder: Unexpected order side, id = "
                        + std::to_string(order_id));
            break;
    }
}

void OrderBook::ReplaceOrder(const Order& old_order, const Order& new_order) {
    if (new_order.price == 0 || new_order.quantity == 0) {
        Logger::Log(LogLevel::Error, "ReplaceOrder: Invalid order");
        return;
    }

    if (old_order.id != new_order.id || old_order.side != new_order.side) {
        CancelOrder(old_order.side, old_order.id);
        NewOrder(new_order);
        Logger::Log(LogLevel::Warning, "Order id = "
            + std::to_string(old_order.id) + " side " + SideToString(old_order.side)
            +" replaced with order id = " + std::to_string(new_order.id)
            + " side " + SideToString(new_order.side));
    }

    auto replace_in_index = [&](auto& index, const Side side) {
        auto it = index.find(new_order.id);
        if (it != index.end()) {
            index.modify(it, [&](Order& o) {
                o.quantity = new_order.quantity;
                o.price = new_order.price; // multi_index автоматически пересортирует
            });
            Logger::Log(LogLevel::Info, "Replaced " + SideToString(side) + " id = "
                + std::to_string(new_order.id));
        } else {
            Logger::Log(LogLevel::Error, "Replace " + SideToString(side) + " failed, id not found: "
                + std::to_string(new_order.id));
        }
    };

    switch (new_order.side) {
        case Side::Ask: {
            auto& ask_index = asks_.get<1>();
            replace_in_index(ask_index, new_order.side);
        } break;

        case Side::Bid: {
            auto& bid_index = bids_.get<1>();
            replace_in_index(bid_index, new_order.side);
        } break;

        default:
            Logger::Log(LogLevel::Error, "ReplaceOrder: Unexpected order side, id = "
                        + std::to_string(new_order.id));
            break;
    }
}

OrderBook::Snapshot OrderBook::GetTopSnapshot(const size_t topN) const {
    Snapshot snapshot;
    snapshot.topBids.reserve(topN);
    snapshot.topAsks.reserve(topN);

    const auto& bid_index = bids_.get<0>();
    size_t count = 0;
    for (auto it = bid_index.begin(); it != bid_index.end() && count < topN; ++it) {
        snapshot.topBids.push_back(&*it);
        ++count;
    }

    const auto& ask_index = asks_.get<0>();
    count = 0;
    for (auto it = ask_index.begin(); it != ask_index.end() && count < topN; ++it) {
        snapshot.topAsks.push_back(&*it);
        ++count;
    }

    return snapshot;
}

const Order* OrderBook::BestBid() const {
    const auto& bid_index = bids_.get<0>();
    return &*bid_index.begin();
}

const Order* OrderBook::BestAsk() const {
    const auto& ask_index = asks_.get<0>();
    return &*ask_index.begin();
}


