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

    struct Order {
        Order() = default;
        Order(ID number, Price p, int qty, bool active);
        ID id = 0;
        Price price = 0;
        int quantity = 0;
        bool active = true;
    };

    struct TopLevel
    {
        Price price;
        int quantity;
        TopLevel(Price p, int qty);
        template <class Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
        {
            ar & price;
            ar & quantity;
        }
    };

    struct Snapshot
    {
        Snapshot(std::vector<TopLevel> bids, std::vector<TopLevel> asks);
        std::vector<TopLevel> topBids;
        std::vector<TopLevel> topAsks;

        template <class Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
        {
            ar & topBids;
            ar & topAsks;
        }
    };

    inline std::vector<uint8_t> SerializeSnapshot(const Snapshot &snap);
    inline Snapshot DeserializeSnapshot(const std::vector<uint8_t>& serialized_snapshot);
}
