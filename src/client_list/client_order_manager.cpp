#include "client_order_manager.h"

void ClientOrderManager::add_bid(ClientId id, OrderId bid_id) {
    orders_[id].bids_.push_back(bid_id);
}

void ClientOrderManager::add_ask(ClientId id, OrderId ask_id) {
    orders_[id].asks_.push_back(ask_id);
}

std::vector<OrderId> ClientOrderManager::get_bids(ClientId id) const {
    auto it = orders_.find(id);
    if (it != orders_.end()) {
        return it->second.bids_;
    }
    return {};
}

std::vector<OrderId> ClientOrderManager::get_asks(ClientId id) const {
    auto it = orders_.find(id);
    if (it != orders_.end()) {
        return it->second.asks_;
    }
    return {};
}

void ClientOrderManager::remove_orders(ClientId id) {
    orders_.erase(id);
}

void ClientOrderManager::remove_bid(ClientId id, OrderId bid_id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) {
        return;
    }

    std::vector<OrderId>& bids = it->second.bids_;
    for (auto curr_it = bids.begin(); curr_it != bids.end(); ++curr_it) {
        if (*curr_it == bid_id) {
            bids.erase(curr_it);
            break;
        }
    }
}

void ClientOrderManager::remove_ask(ClientId id, OrderId ask_id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) {
        return;
    }

    std::vector<OrderId>& asks = it->second.asks_;
    for (auto curr_it = asks.begin(); curr_it != asks.end(); ++curr_it) {
        if (*curr_it == ask_id) {
            asks.erase(curr_it);
            break;
        }
    }
}
