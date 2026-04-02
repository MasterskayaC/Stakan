#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include "../TCP_server/include/session.h"
#include "bid_ask_interface.h"

using ClientId = uint64_t;
using OrderId = common::ID;
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

    /**
     * @brief Retrieves information about the client's existence
     * @param id Client identifier
     * @return true/false about the client's existence
     */
    virtual bool has_client(ClientId id) const = 0;

    // ---------- Order Management ----------
    /**
     * @brief Adds a bid order to a client's list
     * @param id Client identifier
     * @param bid Shared_ptr to the bid order
     */
    virtual void add_bid(ClientId id, std::shared_ptr<common::Order> bid) = 0;

    /**
     * @brief Adds an ask order to a client's list
     * @param id Client identifier
     * @param ask Shared_ptr to the ask order
     */
    virtual void add_ask(ClientId id, std::shared_ptr<common::Order> ask) = 0;

    /**
     * @brief Adds a bid order to a client's list
     * @param id Client identifier
     * @param bid_id Identifier of the bid order
     */
    virtual void remove_bid(ClientId id, OrderId bid_id) = 0;

    /**
     * @brief Adds an ask order to a client's list
     * @param id Client identifier
     * @param ask_id Identifier of the ask order
     */
    virtual void remove_ask(ClientId id, OrderId ask_id) = 0;

    /**
     * @brief Retrieves all bid orders of a client
     * @param id Client identifier
     * @return Vector of bid orders
     */
    virtual std::vector<std::shared_ptr<common::Order>> get_bids(ClientId id) const = 0;

    /**
     * @brief Retrieves all bid orders of a client
     * @param id Client identifier
     * @return Vector of bid orders
     */
    virtual const std::vector<std::shared_ptr<common::Order>>& get_bids_ref(ClientId id) const = 0;

    /**
     * @brief Retrieves all ask orders of a client
     * @param id Client identifier
     * @return Vector of ask orders
     */
    virtual std::vector<std::shared_ptr<common::Order>> get_asks(ClientId id) const = 0;

    /**
     * @brief Retrieves all ask orders of a client
     * @param id Client identifier
     * @return Vector of ask orders
     */
    virtual const std::vector<std::shared_ptr<common::Order>>& get_asks_ref(ClientId id) const = 0;

    // ---------- Subscription Management ----------

    /**
     * @brief Retrieves information about the customer's subscription availability
     * @param id Client identifier
     * @return true/false about the client's subscription
     */
    virtual bool is_subscribed(ClientId id) const = 0;

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
     * @brief Retrieves all subscribed client SessionPtr
     * @return Vector of client SessionPtr
     */
    virtual std::vector<SessionPtr> get_subscribed_sessions() const = 0;

    /**
     * @brief Broadcasts a message to all subscribed clients
     * @param message Binary data to send
     */
    virtual void broadcast_to_subscribed(const std::vector<char>& message) = 0;

    /**
     * @brief Broadcasts a message to a certain client
     * @param id of that certain client
     * @param message Binary data to send
     */
    virtual void broadcast_to_certain(ClientId id, const std::vector<char>& message) = 0;

private:
};

/**
 * @brief Creates a new instance of IClientList.
 * @return A unique pointer owning the newly created IClientList object.
 */
std::unique_ptr<IClientList> makeClientList();
