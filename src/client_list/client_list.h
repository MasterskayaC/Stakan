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

/** Минимальное описание заявки для хранения в списке клиента (сопоставление по id при удалении). */
struct Order {
    OrderId id{};
};

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
    /**
     * @brief Retrieves a client's session.
     * @param id Client identifier.
     * @return Shared pointer to the session, or nullptr if not found.
     */
    virtual SessionPtr get_session(ClientId id) const = 0;
    /**
     * @brief Returns the total number of client records in the list.
     * @return Number of entries (including clients without an open socket).
     */
    virtual size_t size() const = 0;
    /**
     * @brief Retrieves all sessions that are currently attached to clients.
     * @return Vector of session pointers (may contain nulls if only orders exist).
     */
    virtual std::vector<SessionPtr> get_all_sessions() const = 0;
    /**
     * @brief Checks whether a client record exists.
     * @param id Client identifier.
     * @return true if the client exists in the list.
     */
    virtual bool has_client(ClientId id) const = 0;

    // --------- Orders ---------
    /**
     * @brief Appends a bid order for an existing client.
     * @param id Client identifier.
     * @param bid Order object (must not be null).
     */
    virtual void add_bid(ClientId id, std::shared_ptr<Order> bid) = 0;
    /**
     * @brief Appends an ask order for an existing client.
     * @param id Client identifier.
     * @param ask Order object (must not be null).
     */
    virtual void add_ask(ClientId id, std::shared_ptr<Order> ask) = 0;
    /**
     * @brief Removes a bid with the given order id for the client.
     * @param id Client identifier.
     * @param bid_id Order id to remove.
     */
    virtual void remove_bid(ClientId id, OrderId bid_id) = 0;
    /**
     * @brief Removes an ask with the given order id for the client.
     * @param id Client identifier.
     * @param ask_id Order id to remove.
     */
    virtual void remove_ask(ClientId id, OrderId ask_id) = 0;
    /**
     * @brief Returns a copy of all bids for the client.
     */
    virtual std::vector<std::shared_ptr<Order>> get_bids(ClientId id) const = 0;
    /**
     * @brief Returns a const reference to bids (throws if client missing).
     */
    virtual const std::vector<std::shared_ptr<Order>>& get_bids_ref(ClientId id) const = 0;
    /**
     * @brief Returns a copy of all asks for the client.
     */
    virtual std::vector<std::shared_ptr<Order>> get_asks(ClientId id) const = 0;
    /**
     * @brief Returns a const reference to asks (throws if client missing).
     */
    virtual const std::vector<std::shared_ptr<Order>>& get_asks_ref(ClientId id) const = 0;

    // --------- Subscriptions / broadcast ---------
    /**
     * @brief Returns whether the client is marked as subscribed to snapshots.
     */
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
    /**
     * @brief Returns identifiers of clients subscribed to snapshots.
     */
    virtual std::vector<ClientId> get_subscribed_clients() const = 0;
    /**
     * @brief Returns active TCP sessions for subscribed clients only.
     */
    virtual std::vector<SessionPtr> get_subscribed_sessions() const = 0;
    /**
     * @brief Sends a binary payload to every subscribed session (via Session::SendMsg).
     */
    virtual void broadcast_to_subscribed(const std::vector<char>& message) = 0;
    /**
     * @brief Looks up client id by raw session pointer (for disconnect handling).
     */
    virtual std::optional<ClientId> find_client_id_by_session(const Session* session) const = 0;
};
std::unique_ptr<IClientList> makeClientList();
