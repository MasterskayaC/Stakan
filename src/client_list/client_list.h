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

/**
 * @brief Interface for managing all client-related data in one place.
 *
 * Combines session management, order tracking (bids/asks), and subscription
 * state used for snapshot delivery.
 */
class IClientList {
public:
    virtual ~IClientList() = default;

    // --------- Session management ---------
    /**
     * @brief Registers or replaces the active TCP session for a client.
     * @param id Client identifier.
     * @param session Active session handle.
     */
    virtual void add_session(ClientId id, SessionPtr session) = 0;
    /**
     * @brief Drops a client and all associated state from the list.
     * @param id Client identifier.
     */
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
    /**
     * @brief Subscribes a client to snapshot broadcasts.
     * @param id Client identifier.
     * @note Помечает клиента как подписанного на рассылку снэпшотов (без неявного создания записи).
     */
    virtual void subscribe(ClientId id) = 0;
    /**
     * @brief Unsubscribes a client from snapshot broadcasts.
     * @param id Client identifier.
     * @note Помечает клиента как отписанного от рассылки снэпшотов.
     */
    virtual void unsubscribe(ClientId id) = 0;
    /** Вернуть идентификаторы клиентов, которым надо отправлять снэпшоты. */
    virtual std::vector<ClientId> get_subscribed_clients() const = 0;
    /** Вернуть активные сессии только подписанных клиентов. */
    virtual std::vector<SessionPtr> get_subscribed_sessions() const = 0;
    /** Отправить бинарное сообщение всем подписанным клиентам. */
    virtual void broadcast_to_subscribed(const std::vector<char>& message) = 0;
    virtual std::optional<ClientId> find_client_id_by_session(const Session* session) const = 0;
};
std::unique_ptr<IClientList> makeClientList();
