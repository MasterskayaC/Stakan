#include "client_list.h"

ClientList::ClientList()
    : session_manager_(std::make_unique<ClientSessionManager>()),
    order_manager_(std::make_unique<ClientOrderManager>()) {}

void ClientList::add_session(ClientId id, SessionPtr session) {
    std::lock_guard lock(mutex_);
    session_manager_->add_session(id, session);
    // The new client is not subscribed by default
}

void ClientList::remove_session(ClientId id) {
    std::lock_guard lock(mutex_);
    session_manager_->remove_session(id);
    order_manager_->remove_orders(id);   
    subscribed_.erase(id);               
}

SessionPtr ClientList::get_session(ClientId id) const {
    std::lock_guard lock(mutex_);
    return session_manager_->get_session(id);
}

size_t ClientList::size() const {
    std::lock_guard lock(mutex_);
    return session_manager_->size();
}

std::vector<SessionPtr> ClientList::get_all_sessions() const {
    std::lock_guard lock(mutex_);
    return session_manager_->get_all_sessions();
}

void ClientList::add_bid(ClientId id, OrderId bid_id) {
    std::lock_guard lock(mutex_);
    order_manager_->add_bid(id, bid_id);
}

void ClientList::add_ask(ClientId id, OrderId ask_id) {
    std::lock_guard lock(mutex_);
    order_manager_->add_ask(id, ask_id);
}

std::vector<OrderId> ClientList::get_bids(ClientId id) const {
    std::lock_guard lock(mutex_);
    return order_manager_->get_bids(id);
}

std::vector<OrderId> ClientList::get_asks(ClientId id) const {
    std::lock_guard lock(mutex_);
    return order_manager_->get_asks(id);
}

void ClientList::subscribe(ClientId id) {
    std::lock_guard lock(mutex_);
    if (session_manager_->get_session(id)) {
        subscribed_.insert(id);
    }
}

void ClientList::unsubscribe(ClientId id) {
    std::lock_guard lock(mutex_);
    subscribed_.erase(id);
}

std::vector<ClientId> ClientList::get_subscribed_clients() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<ClientId>(subscribed_.begin(), subscribed_.end());
}

void ClientList::broadcast_to_subscribed(const std::vector<char>& message) {
    std::vector<ClientId> subscribed_ids;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribed_ids.assign(subscribed_.begin(), subscribed_.end());
    }

    for (ClientId id : subscribed_ids) {
        SessionPtr session;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            session = session_manager_->get_session(id);
        }

        if (session) {
            std::string msg_str(message.data(), message.size());
            //plug
            //session->SendMsg(msg_str);
        }
        else {
            // If there is no session, delete the ID from the subscription
            std::lock_guard<std::mutex> lock(mutex_);
            subscribed_.erase(id);
        }
    }
}
