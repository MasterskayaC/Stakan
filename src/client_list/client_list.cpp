#include "client_list.h"

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
    mutable std::mutex mutex_;

public:
    void add_session(ClientId id, SessionPtr session) override {
        return;
    }

    void remove_session(ClientId id) override {
        return;
    }

    SessionPtr get_session(ClientId id) const override {
        SessionPtr res;
        try {
            ClientContext cc = clients_.at(id);
            res = cc.session;
        } catch (const std::out_of_range&) {
            throw std::runtime_error("Client " + std::to_string(id) + " not found");
        }
        return res;
    }

    size_t size() const override {
        return 0;
    }

    std::vector<SessionPtr> get_all_sessions() const override {
        std::vector<SessionPtr> tmp;
        return tmp;
    }

    bool has_client(ClientId id) const override {
        return false;
    }

    void add_bid(ClientId id, std::shared_ptr<common::Order> bid) override {
        return;
    }

    void add_ask(ClientId id, std::shared_ptr<common::Order> ask) override {
        return;
    }

    void remove_bid(ClientId id, OrderId bid_id) override {
        return;
    }

    void remove_ask(ClientId id, OrderId ask_id) override {
        return;
    }

    std::vector<std::shared_ptr<common::Order>> get_bids(ClientId id) const override {
        std::vector<std::shared_ptr<common::Order>> tmp;
        return tmp;
    }

    const std::vector<std::shared_ptr<common::Order>>& get_bids_ref(ClientId id) const override {
        throw std::out_of_range("Client not found");
    }

    std::vector<std::shared_ptr<common::Order>> get_asks(ClientId id) const override {
        std::vector<std::shared_ptr<common::Order>> tmp;
        return tmp;
    }

    const std::vector<std::shared_ptr<common::Order>>& get_asks_ref(ClientId id) const override {
        throw std::out_of_range("Client not found");
    }

    bool is_subscribed(ClientId id) const override {
        return false;
    }

    void subscribe(ClientId id) override {
        return;
    }

    void unsubscribe(ClientId id) override {
        return;
    }

    std::vector<ClientId> get_subscribed_clients() const override {
        std::vector<ClientId> tmp;
        return tmp;
    }

    std::vector<SessionPtr> get_subscribed_sessions() const override {
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
        std::string str(message.begin(), message.end());
        for (SessionPtr s : sub_sessions) {
            s->SendMsg(str);
        }
    }

    void broadcast_to_certain(ClientId id, const std::vector<char>& message) override {
        std::string str(message.begin(), message.end());
        SessionPtr s = get_session(id);
        s->SendMsg(str);
    }
};
