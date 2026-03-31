#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <format>

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
    };

    struct Snapshot
    {
        Snapshot() = default;
        Snapshot(std::array<common::Order, topN> bids, std::array<common::Order, topN> asks);
        std::array<common::Order, topN> topBids;
        std::array<common::Order, topN>  topAsks;

        std::string operator()();
        std::vector<char> Serialize() const;
        static Snapshot Deserialize(const std::vector<char>& data);
    };
    
    std::string Snapshot::operator()() {
        std::string result = "Top Bids:\n";
        for (const auto& bid : topBids) {
            if(bid.id == 0) break;
            result += std::format("Price: {}, Quantity: {}\n", bid.price, bid.quantity);
        }
        result += "Top Asks:\n";
        for (const auto& ask : topAsks) {
            if(ask.id == 0) break;
            result += std::format("Price: {}, Quantity: {}\n", ask.price, ask.quantity);
        }
        return result;
    }

    std::vector<char> Snapshot::Serialize() const {
        size_t size = sizeof(Snapshot);
        std::vector<char> buffer(size);
        std::memcpy(buffer.data(), &topBids, sizeof (topBids));
        std::memcpy(buffer.data() + sizeof (topBids), &topAsks, sizeof(topAsks));
        return buffer;
    }

    Snapshot Snapshot::Deserialize(const std::vector<char>& data) {
        if (data.size() < sizeof(Snapshot)) {
            throw std::runtime_error("Deserialization error: incorrect data size");
        }
        Snapshot result;
        std::memcpy(&result.topBids, data.data(), sizeof(topBids));
        std::memcpy(&result.topAsks, data.data() + sizeof(topBids), sizeof(topAsks));
        return result;
    }
}
