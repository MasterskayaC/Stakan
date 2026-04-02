#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <unordered_set>
#include <vector>

// Контракт хранилища клиентов: сессии, заявки и подписки на рассылку.
using ClientId = uint64_t;
using OrderId = uint64_t;
class Session;
using SessionPtr = std::shared_ptr<Session>;
struct Order;

class IClientList {
public:
    virtual ~IClientList() = default;
    virtual void add_session(ClientId id, SessionPtr session) = 0;
    virtual void remove_session(ClientId id) = 0;
    virtual SessionPtr get_session(ClientId id) const = 0;
    virtual size_t size() const = 0;
    virtual std::vector<SessionPtr> get_all_sessions() const = 0;
    virtual bool has_client(ClientId id) const = 0;
    virtual void add_bid(ClientId id, std::shared_ptr<Order> bid) = 0;
    virtual void add_ask(ClientId id, std::shared_ptr<Order> ask) = 0;
    virtual void remove_bid(ClientId id, OrderId bid_id) = 0;
    virtual void remove_ask(ClientId id, OrderId ask_id) = 0;
    virtual std::vector<std::shared_ptr<Order>> get_bids(ClientId id) const = 0;
    virtual const std::vector<std::shared_ptr<Order>>& get_bids_ref(ClientId id) const = 0;
    virtual std::vector<std::shared_ptr<Order>> get_asks(ClientId id) const = 0;
    virtual const std::vector<std::shared_ptr<Order>>& get_asks_ref(ClientId id) const = 0;
    virtual bool is_subscribed(ClientId id) const = 0;
    virtual void subscribe(ClientId id) = 0;
    virtual void unsubscribe(ClientId id) = 0;
    virtual std::vector<ClientId> get_subscribed_clients() const = 0;
    virtual std::vector<SessionPtr> get_subscribed_sessions() const = 0;
    virtual void broadcast_to_subscribed(const std::vector<char>& message) = 0;
    virtual std::optional<ClientId> find_client_id_by_session(const Session* session) const = 0;
};
std::unique_ptr<IClientList> makeClientList();
