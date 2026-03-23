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
    using Price = int64_t;


    enum class Side {Bid, Ask};

    template<class Archive>
    void serialize(Archive& ar, Side& side, [[maybe_unused]] const unsigned int version){
        ar << side;
    }


    std::string SideToString (Side side);

    struct Order {
        Order() = default;
        Order(ID number, Side s, Price p, int qty);
        ID id = 0;
        Side side = Side::Ask;
        Price price = 0;
        int quantity = 0;

        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version){
            ar << id;
            ar << side;
            ar << price;
            ar << quantity;
        }
    };

    struct Snapshot {
        Snapshot() = default;
        Snapshot(std::vector<const Order*> bids, std::vector<const Order*> asks);
        std::vector<const Order*> topBids;
        std::vector<const Order*> topAsks;
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