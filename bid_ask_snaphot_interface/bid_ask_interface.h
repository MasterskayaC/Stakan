#pragma once
#include <array>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

namespace common
{
    constexpr uint8_t topN = 20;

    using ID = uint64_t;
    using Price = uint64_t;
    struct Order {
        Order() = default;
        Order(ID number, Price p, int qty)
            : id(number), price(p), quantity(qty) {}
        ID id = 0;
        Price price = 0;
        int quantity = 0;

        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version){
            ar << id;
            ar << price;
            ar << quantity;
        }
    };

    struct Snapshot
    {
        Snapshot() = default;
        Snapshot(std::array<common::Order, topN> bids, std::array<common::Order, topN> asks);
        std::array<common::Order, topN> topBids;
        std::array<common::Order, topN>  topAsks;

        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version){
            ar << topBids;
            ar << topAsks;
        }
    };

    // @brief Функции для се/десериализации snapshot
    inline std::vector<uint8_t> SerializeSnapshot(const Snapshot &snap);
    inline Snapshot DeserializeSnapshot(const std::vector<uint8_t>& serialized_snapshot);
    
}
