#include <catch2/catch_test_macros.hpp>

#include "inc.h"

TEST_CASE("Order") {
    SECTION("Get price empty order") {
        common::Order o1;
        auto price1 = o1.get_price();
        REQUIRE(price1 == 0);
    }

    SECTION("Get price order") {
        common::Order o2(1, 2, 3);
        auto price2 = o2.get_price();
        REQUIRE(price2 == 2);
    }
}

TEST_CASE("Snapshot serialize") {
    SECTION("Empty snapshot") {
        common::Snapshot empty_snap;
        auto serialize_snap = empty_snap.serialize();
        REQUIRE(serialize_snap.size() == sizeof(common::Snapshot));

        common::Snapshot result_snap = common::Snapshot::deserialize(serialize_snap);
        REQUIRE(empty_snap == result_snap);
    }

    SECTION("Snapshot") {
        common::Snapshot snap;
        for (size_t i = 0; i < common::topN; i++) {
            snap.topBids[i] = common::Order(i + 1, i + 3, i + 5);
            snap.topAsks[i] = common::Order(i + 2, i + 4, i + 6);
        }
        auto serialize_snap = snap.serialize();

        REQUIRE(serialize_snap.size() == sizeof(common::Snapshot));

        common::Snapshot result_snap = common::Snapshot::deserialize(serialize_snap);

        REQUIRE(snap == result_snap);
    }
}

TEST_CASE("Snapshot get prices functions") {
    SECTION("Empty snapshot") {
        common::Snapshot empty_snap;
        std::array<double, common::topN> p{};
        auto bid_prices = empty_snap.get_bid_prices();
        auto ask_prices = empty_snap.get_ask_prices();
        REQUIRE(p == bid_prices);
        REQUIRE(p == ask_prices);
    }

    SECTION("Snapshot") {
        common::Snapshot snap;
        for (size_t i = 0; i < common::topN; i++) {
            snap.topBids[i] = common::Order(i + 1, i + 3, i + 5);
            snap.topAsks[i] = common::Order(i + 2, i + 4, i + 6);
        }

        std::array<double, common::topN> p1{3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0,
                                            13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0};
        auto bid_prices = snap.get_bid_prices();
        REQUIRE(p1 == bid_prices);

        std::array<double, common::topN> p2{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0, 13.0,
                                            14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0};
        auto ask_prices = snap.get_ask_prices();
        REQUIRE(p2 == ask_prices);
    }
}

TEST_CASE("Snapshot to string function") {
    SECTION("Empty snapshot") {
        common::Snapshot empty_snap;
        std::string str_snap = common::to_string(empty_snap);
        std::string empty_str = "";
        assert(str_snap == empty_str);
    }

    SECTION("Snapshot") {
        common::Snapshot snap;
        for (size_t i = 0; i <= 2; i++) {
            snap.topBids[i] = common::Order(i + 1, i + 3, i + 5);
            snap.topAsks[i] = common::Order(i + 2, i + 4, i + 6);
        }
        std::string str_snap = common::to_string(snap);
        std::string str =
            "Top Bids:\n"
            "Price: 3, Quantity: 5\n"
            "Price: 4, Quantity: 6\n"
            "Price: 5, Quantity: 7\n"
            "Top Asks:\n"
            "Price: 4, Quantity: 6\n"
            "Price: 5, Quantity: 7\n"
            "Price: 6, Quantity: 8\n";
        assert(str_snap == str);
    }
}
