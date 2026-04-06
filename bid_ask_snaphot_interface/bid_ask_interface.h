#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <format>
#include <array>

namespace common
{
    constexpr uint8_t topN = 20;

    using ID = uint64_t;
    using Price = uint64_t;
    using Quantity = uint64_t;

    struct Order {
        Order() = default;
        Order(ID number, Price p, int qty)
            : id(number), price(p), quantity(qty) {}
        ID id = 0;
        Price price = 0;
        Quantity quantity = 0;

        double get_price() const;
        auto list_to_compare() const;
    };

    double Order::get_price() const {
        return static_cast<double>(price);
    }

    auto Order::list_to_compare() const {
        return std::tie(id, price, quantity);
    }

    bool operator== (const Order& lhs, const Order& rhs) {
        return lhs.list_to_compare() == rhs.list_to_compare();
    }

    struct Snapshot
    {
        Snapshot() = default;
        Snapshot(std::array<common::Order, topN> bids, std::array<common::Order, topN> asks);
        std::array<common::Order, topN> topBids;
        std::array<common::Order, topN>  topAsks;

        static std::array<double, topN> get_prices(const std::array<common::Order, topN>& arr);
        std::array<double, topN> get_bid_prices() const;
        std::array<double, topN> get_ask_prices() const;
        std::vector<char> serialize() const;
        static Snapshot deserialize(const std::vector<char>& data);
    };    

    std::vector<char> Snapshot::serialize() const {
        size_t size = sizeof(Snapshot);
        std::vector<char> buffer(size);
        std::memcpy(buffer.data(), &topBids, sizeof (topBids));
        std::memcpy(buffer.data() + sizeof (topBids), &topAsks, sizeof(topAsks));
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
        if (snapshot.topBids[0].id == 0 && snapshot.topAsks[0].id == 0) return "";
        std::string result = "Top Bids:\n";
        for (const auto& bid : snapshot.topBids) {
            if(bid.id == 0) break;
            result += std::format("Price: {}, Quantity: {}\n", bid.price, bid.quantity);
        }
        result += "Top Asks:\n";
        for (const auto& ask : snapshot.topAsks) {
            if(ask.id == 0) break;
            result += std::format("Price: {}, Quantity: {}\n", ask.price, ask.quantity);
        }
        return result;
    }

    std::array<double, topN> Snapshot::get_prices(const std::array<common::Order, topN>& arr) {
        std::array<double, topN> result;

        //TODO (simanov artem): rework if need another delimiter;

        for (size_t i = 0; i < topN; i++) {
            result[i] = static_cast<double>(arr[i].price);
        }
        return result;
    }

    std::array<double, topN> Snapshot::get_bid_prices() const {
        return get_prices(topBids);
    }

    std::array<double, topN> Snapshot::get_ask_prices() const {
        return get_prices(topAsks);
    }

    bool operator== (const Snapshot& lhs, const Snapshot& rhs)  {
        return lhs.topBids == rhs.topBids && lhs.topAsks == rhs.topAsks;
    }
}
