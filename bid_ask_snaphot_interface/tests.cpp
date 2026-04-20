#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "bid_ask_interface.h"

TEST_CASE("Order") {
    SECTION("Get price empty order") {
        common::Order o1;
        auto price1_dispay = o1.get_price();
        REQUIRE(o1.price == 0);
        REQUIRE(price1_dispay == Catch::Approx(0.00 / common::PRICE_DELIMETER));
    }

    SECTION("Get price order") {
        common::Order o2(1, 2, 3);
        auto price2_dispay = o2.get_price();
        REQUIRE(o2.price == 2);
        REQUIRE(price2_dispay == Catch::Approx(2.00 / common::PRICE_DELIMETER));
    }
}

TEST_CASE("Snapshot serialize") {
    SECTION("Empty snapshot") {
        common::Snapshot empty_snap;
        auto serialize_snap = empty_snap.serialize();
        REQUIRE(serialize_snap.size() == sizeof(common::Snapshot) + common::kByteForMsgType);

        common::Snapshot result_snap = common::Snapshot::deserialize(serialize_snap);
        REQUIRE(empty_snap == result_snap);
    }

    SECTION("Snapshot") {
        common::Snapshot snap;
        for (size_t i = 0; i < common::topN; i++) {
            snap.topBids[i] = common::Order(i + 1, i + 3, i + 5);
            snap.topAsks[i] = common::Order(i + 100, i + 4, i + 6);
        }
        auto serialize_snap = snap.serialize();

        REQUIRE(serialize_snap.size() == sizeof(common::Snapshot) + common::kByteForMsgType);

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
            snap.topAsks[i] = common::Order(i + 100, i + 4, i + 6);
        }

        std::array<double, common::topN> p1{0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.11, 0.12,
                                            0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, 0.21, 0.22};
        auto bid_prices = snap.get_bid_prices();
        REQUIRE(p1 == bid_prices);

        std::array<double, common::topN> p2{0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.11, 0.12, 0.13,
                                            0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, 0.21, 0.22, 0.23};
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
            snap.topAsks[i] = common::Order(i + 100, i + 4, i + 6);
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

TEST_CASE("MDUpdate serialization") {
    auto areEqual = [](const common::MDUpdate& lhs, const common::MDUpdate& rhs) {
        auto tieFields = [](const common::MDUpdate& mdup) {
            return std::tie(mdup.best_price_,
                            mdup.bids_nums_,
                            mdup.bids_items_nums_,
                            mdup.asks_nums_,
                            mdup.askss_items_nums_,
                            mdup.all_orders_nums_);
        };
        return tieFields(lhs) == tieFields(rhs);
    };

    auto [mdup, description] = GENERATE(
        table<common::MDUpdate, std::string>({{{1, 2, 3, 4, 5, 6}, "Сonsecutive values"},
                                              {{0, 0, 0, 0, 0, 0}, "Zero values"},
                                              {{UINT64_MAX, 1, UINT64_MAX, 0, UINT64_MAX, 7}, "Max and mix values"}}));

    DYNAMIC_SECTION("Testing case: " << description) {
        std::vector<char> serialized = mdup.serialize();
        common::MDUpdate deserialized = common::MDUpdate::deserialize(serialized);

        const size_t expected_size = sizeof(common::MDUpdate) + common::kByteForMsgType;

        REQUIRE(serialized.size() == expected_size);
        CHECK(areEqual(mdup, deserialized));
        CHECK(static_cast<int>(serialized[common::kPtrForMsgType]) == 1);
    }
}
