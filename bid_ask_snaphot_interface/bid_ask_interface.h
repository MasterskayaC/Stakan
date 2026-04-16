#pragma once
#include <array>
#include <cstdint>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace common {
constexpr uint8_t topN = 20;

///delimeter for cast order price to double
static const double PRICE_DELIMETER = 100;

using ID = uint64_t;
using Price = uint64_t;
using Quantity = uint64_t;

struct Order {
    Order() = default;
    Order(ID number, Price p, int qty) : id(number), price(p), quantity(qty) {}
    ID id = 0;  /// id must always be strictly greater than 1
    Price price = 0;
    Quantity quantity = 0;
    /**
     * @brief get order's price as double
     * @return price
     */
    double get_price() const;
};

double Order::get_price() const {
    return static_cast<double>(price) / PRICE_DELIMETER;
}

bool operator==(const Order& lhs, const Order& rhs) {
    return lhs.id == rhs.id && lhs.price == rhs.price && lhs.quantity == rhs.quantity;
}

struct Snapshot {
    Snapshot() = default;
    Snapshot(std::array<common::Order, topN> bids, std::array<common::Order, topN> asks);
    std::array<common::Order, topN> topBids;
    std::array<common::Order, topN> topAsks;

    /**
     * @brief basic funtion is get array of double
     * @param array of Orders
     * @return array of double
     */
    static std::array<double, topN> get_prices(const std::array<common::Order, topN>& arr);

    /**
     * @brief get bids prices as array of double
     * @return array of double
     */
    std::array<double, topN> get_bid_prices() const;

    /**
     * @brief get asks prices as array of double
     * @return
     */
    std::array<double, topN> get_ask_prices() const;

    /**
     * @brief serialize snapshor
     * @return vector of char
     */
    std::vector<char> serialize() const;

    /**
     * @brief deserialize snapshot
     * @param binary data array
     * @return snapshot
     */
    static Snapshot deserialize(const std::vector<char>& data);
};

struct MDUpdate {
    Price best_price_;
    Quantity bids_nums_;
    Quantity bids_items_nums_;

    Quantity asks_nums_;
    Quantity askss_items_nums_;

    Quantity all_orders_nums_;

    /**
     * @brief serialize MDUpdate
     * @return vector of char
     */

    std::vector<char> serialize() const {
        return {};
    }

    /**
     * @brief deserialize MDUpdate
     * @param data binary data array
     * @return MDUpdate object
     */
    static MDUpdate deserialize(const std::vector<char>& data) {
        return {};
    }
};

std::vector<char> Snapshot::serialize() const {
    size_t size = sizeof(Snapshot);
    std::vector<char> buffer(size);
    std::memcpy(buffer.data(), &topBids, sizeof(topBids));
    std::memcpy(buffer.data() + sizeof(topBids), &topAsks, sizeof(topAsks));
    return buffer;
}

Snapshot Snapshot::deserialize(const std::vector<char>& data) {
    if (data.size() != sizeof(Snapshot)) {
        throw std::runtime_error("Deserialization error: incorrect data size");
    }
    Snapshot result;
    std::memcpy(&result.topBids, data.data(), sizeof(topBids));
    std::memcpy(&result.topAsks, data.data() + sizeof(topBids), sizeof(topAsks));
    return result;
}

std::string to_string(const Snapshot& snapshot) {
    if (snapshot.topBids[0].id == 0 && snapshot.topAsks[0].id == 0)
        return "";
    std::string result = "Top Bids:\n";
    for (const auto& bid : snapshot.topBids) {
        if (bid.id == 0)
            break;
        result += std::format("Price: {}, Quantity: {}\n", bid.price, bid.quantity);
    }
    result += "Top Asks:\n";
    for (const auto& ask : snapshot.topAsks) {
        if (ask.id == 0)
            break;
        result += std::format("Price: {}, Quantity: {}\n", ask.price, ask.quantity);
    }
    return result;
}

std::array<double, topN> Snapshot::get_prices(const std::array<common::Order, topN>& arr) {
    std::array<double, topN> result;
    for (size_t i = 0; i < topN; i++) {
        result[i] = arr[i].get_price();
    }
    return result;
}

std::array<double, topN> Snapshot::get_bid_prices() const {
    return get_prices(topBids);
}

std::array<double, topN> Snapshot::get_ask_prices() const {
    return get_prices(topAsks);
}

bool operator==(const Snapshot& lhs, const Snapshot& rhs) {
    return lhs.topBids == rhs.topBids && lhs.topAsks == rhs.topAsks;
}
}  /// namespace common
