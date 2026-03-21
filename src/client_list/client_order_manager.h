#pragma once

#include <unordered_map>
#include <vector>

using OrderId = uint64_t;
using ClientId = uint64_t;

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
     * @param bid_id Identifier of the bid order
     */
    virtual void add_bid(ClientId id, OrderId bid_id) = 0;

    /**
     * @brief Adds an ask order to the client's list
     * @param id Client identifier
     * @param ask_id Identifier of the ask order
     */
    virtual void add_ask(ClientId id, OrderId ask_id) = 0;

    /**
     * @brief Retrieves all bid orders of a client
     * @param id Client identifier
     * @return Vector of bid order IDs (empty if client not found)
     */
    virtual std::vector<OrderId> get_bids(ClientId id) const = 0;

    /**
     * @brief Retrieves all ask orders of a client
     * @param id Client identifier
     * @return Vector of ask order IDs (empty if client not found)
     */
    virtual std::vector<OrderId> get_asks(ClientId id) const = 0;

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
 * @brief Concrete implementation of IClientOrderManager using a hash map
 *
 * Stores for each client two vectors: one for bids and one for asks
 * Order IDs are stored in the order they were added
 */
class ClientOrderManager : public IClientOrderManager {
private:
    struct ClientOrders {
        std::vector<OrderId> bids_;
        std::vector<OrderId> asks_;
    };
    std::unordered_map<ClientId, ClientOrders> orders_;

public:
    void add_bid(ClientId id, OrderId bid_id) override;
    void add_ask(ClientId id, OrderId ask_id) override;

    std::vector<OrderId> get_bids(ClientId id) const override;
    std::vector<OrderId> get_asks(ClientId id) const override;

    void remove_orders(ClientId id) override;

    void remove_bid(ClientId id, OrderId bid_id) override;
    void remove_ask(ClientId id, OrderId ask_id) override;
};