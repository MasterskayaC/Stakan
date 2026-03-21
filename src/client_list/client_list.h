#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

#include "client_order_manager.h"
#include "client_session_manager.h"

using ClientId = uint64_t;
using OrderId = uint64_t;
class Session;
using SessionPtr = std::shared_ptr<Session>;

/**
 * @brief Interface for managing all client-related data
 *
 * Combines session management, order tracking, and subscription handling
 * Provides a unified API for the server to manage clients and broadcast updates
 */
class IClientList {
public:
    virtual ~IClientList() = default;

    // ---------- Session Management ----------
    /**
     * @brief Adds a session for a client
     * @param id Client identifier
     * @param session Shared pointer to the session object
     */
    virtual void add_session(ClientId id, SessionPtr session) = 0;

    /**
     * @brief Removes a client's session
     * @param id Client identifier
     */
    virtual void remove_session(ClientId id) = 0;

    /**
     * @brief Retrieves a client's session
     * @param id Client identifier
     * @return Shared pointer to the session, or nullptr if not found
     */
    virtual SessionPtr get_session(ClientId id) const = 0;

    /**
     * @brief Returns the total number of active sessions
     * @return Number of sessions
     */
    virtual size_t size() const = 0;

    /**
     * @brief Retrieves all active sessions
     * @return Vector of session pointers
     */
    virtual std::vector<SessionPtr> get_all_sessions() const = 0;

    // ---------- Order Management ----------
    /**
     * @brief Adds a bid order to a client's list
     * @param id Client identifier
     * @param bid_id Identifier of the bid order
     */
    virtual void add_bid(ClientId id, OrderId bid_id) = 0;

    /**
     * @brief Adds an ask order to a client's list
     * @param id Client identifier
     * @param ask_id Identifier of the ask order
     */
    virtual void add_ask(ClientId id, OrderId ask_id) = 0;

    /**
     * @brief Retrieves all bid orders of a client
     * @param id Client identifier
     * @return Vector of bid order IDs
     */
    virtual std::vector<OrderId> get_bids(ClientId id) const = 0;

    /**
     * @brief Retrieves all ask orders of a client
     * @param id Client identifier
     * @return Vector of ask order IDs
     */
    virtual std::vector<OrderId> get_asks(ClientId id) const = 0;

    // ---------- Subscription Management ----------
    /**
     * @brief Subscribes a client to order book updates
     * @param id Client identifier
     */
    virtual void subscribe(ClientId id) = 0;

    /**
     * @brief Unsubscribes a client from order book updates
     * @param id Client identifier
     */
    virtual void unsubscribe(ClientId id) = 0;

    /**
     * @brief Retrieves all subscribed client IDs
     * @return Vector of client IDs
     */
    virtual std::vector<ClientId> get_subscribed_clients() const = 0;

    /**
     * @brief Broadcasts a message to all subscribed clients
     * @param message Binary data to send
     */
    virtual void broadcast_to_subscribed(const std::vector<char>& message) = 0;
};

/**
 * @brief Concrete implementation of IClientList with thread safety
 *
 * Combines a session manager, an order manager, and a set of subscribed clients
 * All public methods are guarded by a mutex to ensure thread safety
 */
class ClientList : public IClientList {
private:
    mutable std::mutex mutex_;
    std::unique_ptr<IClientSessionManager> session_manager_;
    std::unique_ptr<IClientOrderManager> order_manager_;
    std::unordered_set<ClientId> subscribed_;
public:
    ClientList();

    // ---------- Session Management ----------
    void add_session(ClientId id, SessionPtr session) override;
    void remove_session(ClientId id) override;
    SessionPtr get_session(ClientId id) const override;
    size_t size() const override;
    std::vector<SessionPtr> get_all_sessions() const override;

    // ---------- Order Management ----------
    void add_bid(ClientId id, OrderId bid_id) override;
    void add_ask(ClientId id, OrderId ask_id) override;
    std::vector<OrderId> get_bids(ClientId id) const override;
    std::vector<OrderId> get_asks(ClientId id) const override;

    // ---------- Subscription Management ----------
    void subscribe(ClientId id) override;
    void unsubscribe(ClientId id) override;
    std::vector<ClientId> get_subscribed_clients() const override;
    void broadcast_to_subscribed(const std::vector<char>& message) override;
};