#include "client_list.h"

#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "bid_ask_interface.h"
#include "command_queue.h"

class ClientList : public IClientList {
private:
    struct OrderEntry {
        std::shared_ptr<common::Order> order;
        std::list<common::ID>::iterator it;
    };

    struct ClientContext {
        SessionPtr session;

        std::unordered_map<common::ID, OrderEntry> bids;
        std::unordered_map<common::ID, OrderEntry> asks;
        std::list<common::ID> bids_in_order;
        std::list<common::ID> asks_in_order;
        bool subscribed = false;
    };

    std::unordered_map<ClientId, ClientContext> clients_;
    std::unordered_map<const Session*, ClientId> session_to_client_;
    mutable std::mutex mutex_;

public:
    void add_session(ClientId id, SessionPtr session) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end() && it->second.session) {
            session_to_client_.erase(it->second.session.get());
            std::cout << "Client with id: " << id << " was recreated\n";
            // When there's a common logger replace the message
        }
        clients_[id].session = std::move(session);
        if (clients_[id].session) {
            session_to_client_[clients_[id].session.get()] = id;
        }
    }

    void remove_session(ClientId id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end() && it->second.session) {
            session_to_client_.erase(it->second.session.get());
            std::cout << "Session with id: " << id << " was deleted\n";
            // When there's a common logger replace the message
        }
        clients_.erase(id);
        std::cout << "Client with id: " << id << " was deleted\n";
        // When there's a common logger replace the message
    }

    SessionPtr get_session(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.at(id);
        return it.session;
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return clients_.size();
    }

    std::vector<SessionPtr> get_all_sessions() const override {
        std::vector<SessionPtr> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        tmp.reserve(clients_.size());
        for (const auto& [id, ctx] : clients_) {
            (void)id;
            if (ctx.session) {
                tmp.push_back(ctx.session);
            }
        }
        return tmp;
    }

    bool has_client(ClientId id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return clients_.find(id) != clients_.end();
    }

    void add_bid(ClientId id, std::shared_ptr<common::Order> bid) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& it = clients_.at(id);
        it.bids_in_order.push_back(bid->id);
        auto last = std::prev(it.bids_in_order.end());
        it.bids[id] = {std::move(bid), last};
    }

    void add_ask(ClientId id, std::shared_ptr<common::Order> ask) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& it = clients_.at(id);
        it.asks_in_order.push_back(ask->id);
        auto last = std::prev(it.asks_in_order.end());
        it.asks[id] = {std::move(ask), last};
    }

    void remove_bid(ClientId id, std::optional<common::ID> order_id = std::nullopt) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& it = clients_.at(id);
        if (!it.bids.empty()) {
            if (order_id) {
                auto order_it = it.bids.find(order_id.value());
                if (order_it == it.bids.end()) {
                    std::cout << "Client with id: " << id << " doesn't have and order with id: " << order_id.value()
                              << '\n';
                    // change for logger later
                    return;
                }
                it.bids_in_order.erase(order_it->second.it);
                it.bids.erase(order_it);
            } else {
                common::ID order_id = it.bids_in_order.back();
                it.bids_in_order.pop_back();
                it.bids.erase(order_id);
            }
        }
    }

    void remove_ask(ClientId id, std::optional<common::ID> order_id = std::nullopt) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& it = clients_.at(id);
        if (!it.asks.empty()) {
            if (order_id) {
                auto order_it = it.asks.find(order_id.value());
                if (order_it == it.asks.end()) {
                    std::cout << "Client with id: " << id << " doesn't have and order with id: " << order_id.value()
                              << '\n';
                    // change for logger later
                    return;
                }
                it.asks_in_order.erase(order_it->second.it);
                it.asks.erase(order_it);
            } else {
                common::ID order_id = it.asks_in_order.back();
                it.asks_in_order.pop_back();
                it.asks.erase(order_id);
            }
        }
    }

    std::vector<std::shared_ptr<common::Order>> get_bids(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::vector<std::shared_ptr<common::Order>> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.at(id);
        for (common::ID order_id : it.bids_in_order) {
            tmp.push_back(it.bids.at(order_id).order);
        }
        return tmp;
    }

    std::vector<std::shared_ptr<common::Order>> get_asks(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::vector<std::shared_ptr<common::Order>> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        const ClientContext& it = clients_.at(id);
        for (common::ID order_id : it.asks_in_order) {
            tmp.push_back(it.asks.at(order_id).order);
        }
        return tmp;
    }

    bool is_subscribed(ClientId id) const override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        const ClientContext& it = clients_.at(id);
        return it.subscribed;
    }

    void subscribe(ClientId id) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        ClientContext& it = clients_.at(id);
        it.subscribed = true;
    }

    void unsubscribe(ClientId id) override {
        if (!has_client(id))
            throw std::runtime_error("There is no client with id: " + std::to_string(id));
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.at(id);
        it.subscribed = false;
    }

    std::vector<ClientId> get_subscribed_clients() const override {
        std::vector<ClientId> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [id, ctx] : clients_) {
            if (ctx.subscribed) {
                tmp.push_back(id);
            }
        }
        return tmp;
    }

    std::vector<SessionPtr> get_subscribed_sessions() const override {
        std::lock_guard lck(mutex_);
        std::vector<SessionPtr> res;
        res.reserve(clients_.size());
        for (auto [client_id, client_context] : clients_) {
            if (client_context.subscribed) {
                res.push_back(client_context.session);
            }
        }
        return res;
    }

    void broadcast_to_subscribed(const std::vector<char>& message) override {
        std::vector<SessionPtr> sub_sessions = get_subscribed_sessions();
        for (SessionPtr s : sub_sessions) {
            s->SendMsg(message);
        }
    }

    void broadcast_to_certain(ClientId id, const std::vector<char>& message) override {
        SessionPtr s = get_session(id);
        s->SendMsg(message);
    }

    std::optional<ClientId> find_client_id_by_session(const Session* session) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = session_to_client_.find(session);
        if (it == session_to_client_.end()) {
            return std::nullopt;
        }
        return it->second;
    }
};

std::unique_ptr<IClientList> makeClientList() {
    return std::make_unique<ClientList>();
}
