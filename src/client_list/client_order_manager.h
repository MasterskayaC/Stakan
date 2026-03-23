#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

using OrderId = uint64_t;
using ClientId = uint64_t;
struct Order;

/**
 * @brief Interface for managing client orders (bid/ask lists)
 *
 * Provides methods to add, retrieve, and remove orders associated with a specific client
 */
class IClientOrderManager {
public:
    virtual ~IClientOrderManager() = default;

    /**
     * @brief Adds a bid order to the client's list
     * @param id Client identifier
     * @param bid Shared_ptr to the bid order
     */
    virtual void add_bid(ClientId id, std::shared_ptr<Order> bid) = 0;

    /**
     * @brief Adds an ask order to the client's list
     * @param id Client identifier
     * @param ask Shared_ptr to the ask order
     */
    virtual void add_ask(ClientId id, std::shared_ptr<Order> ask) = 0;

    /**
     * @brief Retrieves all bid orders of a client
     * @param id Client identifier
     * @return Vector of bid order IDs (empty if client not found)
     */
    virtual std::vector<OrderId> get_bids(ClientId id) const = 0;

    /**
     * @brief Retrieves all bid orders of a client
     * @param id Client identifier
     * @return Vector of bid order IDs
     */
    virtual const std::vector<OrderId>& get_bids_ref(ClientId id) const = 0;

    /**
     * @brief Retrieves all ask orders of a client
     * @param id Client identifier
     * @return Vector of ask order IDs (empty if client not found)
     */
    virtual std::vector<OrderId> get_asks(ClientId id) const = 0;

    /**
     * @brief Retrieves all ask orders of a client
     * @param id Client identifier
     * @return Vector of ask order IDs
     */
    virtual const std::vector<OrderId>& get_asks_ref(ClientId id) const = 0;

    /**
     * @brief Removes all orders of a client
     * @param id Client identifier
     */
    virtual void remove_orders(ClientId id) = 0;

    /**
     * @brief Removes a specific bid order from a client
     * @param id Client identifier
     * @param bid_id Identifier of the bid order to remove
     */
    virtual void remove_bid(ClientId id, OrderId bid_id) = 0;

    /**
     * @brief Removes a specific ask order from a client
     * @param id Client identifier
     * @param ask_id Identifier of the ask order to remove
     */
    virtual void remove_ask(ClientId id, OrderId ask_id) = 0;
};

/**
 * @brief Creates a new instance of IClientOrderManager.
 * @return A unique pointer owning the newly created IClientOrderManager object.
 */
std::unique_ptr<IClientOrderManager> makeClientOrderManager();
