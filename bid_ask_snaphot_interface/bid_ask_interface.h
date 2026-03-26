#pragma once
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <string>
#include <vector>
#include <sstream>

namespace common
{
    using ID = uint64_t;
    using Price = uint64_t;
    struct Order {
        Order() = default;
        Order(ID number, Price p, int qty);
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
        Snapshot(std::vector<Order> bids, std::vector<Order> asks);
        std::vector<Order> topBids;
        std::vector<Order> topAsks;
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
