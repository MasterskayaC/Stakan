#include "client_list.h"

#include <mutex>
#include <stdexcept>
#include <unordered_map>

#include "command_queue.h"

class ClientList : public IClientList {
private:
    struct ClientContext {
        SessionPtr session;
        std::vector<std::shared_ptr<common::Order>> bids;
        std::vector<std::shared_ptr<common::Order>> asks;
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
        }
        clients_.erase(id);
    }

    SessionPtr get_session(ClientId id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it == clients_.end()) {
            return {};
        }
        return it->second.session;
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
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it == clients_.end()) {
            return;
        }
        it->second.bids.push_back(std::move(bid));
    }

    void add_ask(ClientId id, std::shared_ptr<common::Order> ask) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it == clients_.end()) {
            return;
        }
        it->second.asks.push_back(std::move(ask));
    }

    void remove_bid(ClientId id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(id);
        if (it != clients_.end() && !it->second.bids.empty()) {
            it->second.bids.pop_back();
        }
    }

    void remove_ask(ClientId id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(id);
        if (it != clients_.end() && !it->second.asks.empty()) {
            it->second.asks.pop_back();
        }
    }

    std::vector<std::shared_ptr<common::Order>> get_bids(ClientId id) const override {
        std::vector<std::shared_ptr<common::Order>> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end()) {
            tmp = it->second.bids;
        }
        return tmp;
    }

    const std::vector<std::shared_ptr<common::Order>>& get_bids_ref(ClientId id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end()) {
            return it->second.bids;
        }
        throw std::out_of_range("Client not found");
    }

    std::vector<std::shared_ptr<common::Order>> get_asks(ClientId id) const override {
        std::vector<std::shared_ptr<common::Order>> tmp;
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end()) {
            tmp = it->second.asks;
        }
        return tmp;
    }

    const std::vector<std::shared_ptr<common::Order>>& get_asks_ref(ClientId id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it != clients_.end()) {
            return it->second.asks;
        }
        throw std::out_of_range("Client not found");
    }

    bool is_subscribed(ClientId id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        return it != clients_.end() && it->second.subscribed;
    }

    void subscribe(ClientId id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = clients_.find(id);
        if (it == clients_.end()) {
            return;
        }
        it->second.subscribed = true;
    }

    void unsubscribe(ClientId id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(id);
        if (it != clients_.end()) {
            it->second.subscribed = false;
        }
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
