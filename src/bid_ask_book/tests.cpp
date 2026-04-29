#include <catch2/catch_test_macros.hpp>

#include "bid_ask_book.h"

using namespace server;
using namespace common;

TEST_CASE("NewBid and NewAsk add orders correctly") {
    OrderBook book;

    Order bid1(1, 100, 10);
    Order ask1(2, 105, 5);

    book.NewBid(bid1);
    book.NewAsk(ask1);

    auto snapshot = book.GetTopSnapshot();

    CHECK(snapshot.topBids[0].id == 1);
    CHECK(snapshot.topBids[0].price == 100);

    CHECK(snapshot.topAsks[0].id == 2);
    CHECK(snapshot.topAsks[0].price == 105);
}

TEST_CASE("CancelBid and CancelAsk remove correctly") {
    OrderBook book;

    Order bid1(1, 100, 10);
    Order ask1(2, 105, 5);

    book.NewBid(bid1);
    book.NewAsk(ask1);

    book.CancelBid(1);
    book.CancelAsk(2);

    auto snapshot = book.GetTopSnapshot();

    // после удаления массив остаётся, но элементы дефолтные
    CHECK(snapshot.topBids[0].id == 0);
    CHECK(snapshot.topAsks[0].id == 0);
}

TEST_CASE("ReplaceBid and ReplaceAsk update price and quantity") {
    OrderBook book;

    Order bid1(1, 100, 10);
    Order ask1(2, 105, 5);

    book.NewBid(bid1);
    book.NewAsk(ask1);

    Order new_bid(1, 110, 15);
    Order new_ask(2, 102, 7);

    book.ReplaceBid(bid1, new_bid);
    book.ReplaceAsk(ask1, new_ask);

    auto snapshot = book.GetTopSnapshot();

    CHECK(snapshot.topBids[0].price == 110);
    CHECK(snapshot.topBids[0].quantity == 15);

    CHECK(snapshot.topAsks[0].price == 102);
    CHECK(snapshot.topAsks[0].quantity == 7);
}

TEST_CASE("BestBid and BestAsk return correct orders") {
    OrderBook book;

    book.NewBid(Order(1, 100, 10));
    book.NewBid(Order(2, 105, 5));
    book.NewAsk(Order(3, 110, 20));
    book.NewAsk(Order(4, 108, 8));

    Order bestBid = book.BestBid();
    Order bestAsk = book.BestAsk();

    CHECK(bestBid.price == 105);  // max price
    CHECK(bestAsk.price == 108);  // min price
}

TEST_CASE("Ordering works correctly for bids and asks") {
    OrderBook book;

    // одинаковые цены → проверяем quantity и id
    book.NewBid(Order(1, 100, 10));
    book.NewBid(Order(2, 100, 20)); // должен быть выше (quantity больше)

    book.NewAsk(Order(3, 105, 10));
    book.NewAsk(Order(4, 103, 10)); // должен быть выше (цена меньше)

    auto snapshot = book.GetTopSnapshot();

    CHECK(snapshot.topBids[0].id == 2); // больше quantity
    CHECK(snapshot.topAsks[0].id == 4); // меньше price
}

TEST_CASE("Replace with different id does nothing") {
    OrderBook book;

    Order bid1(1, 100, 10);
    book.NewBid(bid1);

    Order invalid(2, 110, 15);
    book.ReplaceBid(bid1, invalid);

    auto snapshot = book.GetTopSnapshot();

    // ордер не изменился
    CHECK(snapshot.topBids[0].id == 1);
    CHECK(snapshot.topBids[0].price == 100);
}

TEST_CASE("Check GetPricesInfo function"){
    const common::Price PRICE = 4;
    const common::Price RAND_PRICE = 444;

    SECTION("Empty orderBook") {
        server::OrderBook book;
        auto info_bid = book.GetPricesBidsInfo(7);
        REQUIRE(info_bid.ids_.empty());
        REQUIRE(info_bid.quantity_ == 0);

        auto info_ask = book.GetPricesAsksInfo(7);
        REQUIRE(info_ask.ids_.empty());
        REQUIRE(info_ask.quantity_ == 0);
    }

    SECTION ("Add 1 bid") {
        server::OrderBook book;
        common::Order o{ 1, PRICE, 42 };
        book.NewBid(o);
        auto info = book.GetPricesBidsInfo(PRICE);
        REQUIRE(info.ids_[0] == 1);
        REQUIRE(info.quantity_ == 42);
    }

    SECTION ("Add 3 bids with same price") {
        server::OrderBook book;
        common::Order o1{ 1, PRICE, 42 };
        common::Order o2{ 2, PRICE, 5 };
        common::Order o3{ 3, PRICE, 9 };
        book.NewBid(o1);
        book.NewBid(o2);
        book.NewBid(o3);

        auto info = book.GetPricesBidsInfo(PRICE);
        REQUIRE(info.ids_.size() == 3);
        REQUIRE(info.quantity_ == (42 + 5 + 9));
    }

    SECTION ("Add 1 bid, but check random price") {
        server::OrderBook book;
        common::Order o1{ 1, PRICE, 42 };
        book.NewBid(o1);

        auto info = book.GetPricesBidsInfo(RAND_PRICE);
        REQUIRE(info.ids_.empty());
        REQUIRE(info.quantity_ == 0);
    }

    SECTION ("Add 1 ask") {
        server::OrderBook book;
        common::Order o1{ 1, PRICE, 42 };
        book.NewAsk(o1);
        auto info2 = book.GetPricesAsksInfo(PRICE);
        REQUIRE(info2.ids_[0] == 1);
        REQUIRE(info2.quantity_ == 42);
    }

    SECTION ("Add 3 asks with same price") {
        server::OrderBook book;
        common::Order o1{ 1, PRICE, 42 };
        common::Order o2{ 2, PRICE, 5 };
        common::Order o3{ 3, PRICE, 9 };
        book.NewAsk(o1);
        book.NewAsk(o2);
        book.NewAsk(o3);

        auto info = book.GetPricesAsksInfo(PRICE);
        REQUIRE(info.ids_.size() == 3);
        REQUIRE(info.quantity_ == (42 + 5 + 9));
    }

    SECTION("Add bid and ask") {
        server::OrderBook book;

        const common::Price PRICE = 4;
        common::Order o1{ 1, PRICE, 42 };
        book.NewBid(o1);

        common::Order o2{ 2, PRICE, 42 };
        book.NewAsk(o2);
        auto info_bid = book.GetPricesBidsInfo(PRICE);
        auto info_ask = book.GetPricesAsksInfo(PRICE);
        REQUIRE(info_bid.ids_.size() == 1);
        REQUIRE(info_ask.ids_.size() == 1);
        REQUIRE(info_bid.quantity_ == 42);
        REQUIRE(info_ask.quantity_ == 42);
    }
}

TEST_CASE("GenerateMDUpdate total quantities with validation") {
    OrderBook book;

    SECTION("Empty book returns nullopt") {
        auto md_update = book.GenerateMDUpdate();
        CHECK_FALSE(md_update.has_value());
    }

    SECTION("Invalid orders are rejected and don't affect totals") {
        // Try to add Order with zero price and qty
        book.NewBid(Order(1, 0, 10));
        book.NewAsk(Order(2, 100, 0));

        auto md_update = book.GenerateMDUpdate();
        CHECK_FALSE(md_update.has_value());

        // Add correct Orders
        book.NewBid(Order(3, 100, 10));
        book.NewAsk(Order(4, 105, 5));

        md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 10);
        CHECK(md_update->total_ask_qty == 5);
    }
}

TEST_CASE("GenerateMDUpdate total quantities with multiple operations") {
    OrderBook book;

    SECTION("Add bids and asks with same price") {
        book.NewBid(Order(1, 100, 10));
        book.NewBid(Order(2, 100, 20));
        book.NewBid(Order(3, 100, 30));

        book.NewAsk(Order(4, 105, 5));
        book.NewAsk(Order(5, 105, 15));

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 60);
        CHECK(md_update->total_ask_qty == 20);
        CHECK(md_update->best_bid_price == 100);
        CHECK(md_update->best_ask_price == 105);
    }

    SECTION("Cancel operations update totals correctly") {
        book.NewBid(Order(1, 100, 10));
        book.NewBid(Order(2, 101, 20));
        book.NewAsk(Order(3, 105, 5));
        book.NewAsk(Order(4, 106, 8));

        book.CancelBid(1);
        book.CancelAsk(3);

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 20);
        CHECK(md_update->total_ask_qty == 8);
        CHECK(md_update->best_bid_price == 101);
        CHECK(md_update->best_ask_price == 106);
    }

    SECTION("Cancel non-existent orders don't affect totals") {
        book.NewBid(Order(1, 100, 10));
        book.NewAsk(Order(2, 105, 5));

        book.CancelBid(999);
        book.CancelAsk(999);

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 10);
        CHECK(md_update->total_ask_qty == 5);
    }
}

TEST_CASE("GenerateMDUpdate total quantities with replace operations") {
    OrderBook book;

    SECTION("Replace bid with different quantity") {
        book.NewBid(Order(1, 100, 10));
        book.NewAsk(Order(2, 105, 5));

        Order new_bid(1, 110, 25);
        book.ReplaceBid(Order(1, 100, 10), new_bid);

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 25);
        CHECK(md_update->total_ask_qty == 5);
        CHECK(md_update->best_bid_price == 110);
    }

    SECTION("Replace ask with different quantity") {
        book.NewBid(Order(1, 100, 10));
        book.NewAsk(Order(2, 105, 5));

        Order new_ask(2, 100, 15);
        book.ReplaceAsk(Order(2, 105, 5), new_ask);

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 10);
        CHECK(md_update->total_ask_qty == 15);
        CHECK(md_update->best_ask_price == 100);
    }

    SECTION("Replace with invalid order (zero price or quantity)") {
        book.NewBid(Order(1, 100, 10));
        book.NewAsk(Order(2, 105, 5));

        // Try to replace to Order with zero price and qty
        book.ReplaceBid(Order(1, 100, 10), Order(1, 0, 25));
        book.ReplaceAsk(Order(2, 105, 5), Order(2, 105, 0));

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 10);
        CHECK(md_update->total_ask_qty == 5);
        CHECK(md_update->best_bid_price == 100);
        CHECK(md_update->best_ask_price == 105);
    }

    SECTION("Replace with different id does nothing") {
        book.NewBid(Order(1, 100, 10));

        book.ReplaceBid(Order(1, 100, 10), Order(2, 110, 15));

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 10);
        CHECK(md_update->best_bid_price == 100);
        CHECK(md_update->best_bid_qty == 10);
    }

    SECTION("Replace non-existent order") {
        book.NewBid(Order(1, 100, 10));

        book.ReplaceBid(Order(999, 100, 10), Order(999, 110, 15));

        auto md_update = book.GenerateMDUpdate();
        REQUIRE(md_update.has_value());
        CHECK(md_update->total_bid_qty == 10);
        CHECK(md_update->best_bid_price == 100);
    }
}

TEST_CASE("GenerateMDUpdate total quantities with mixed operations sequence") {
    OrderBook book;

    book.NewBid(Order(1, 100, 10));  // total_bid = 10
    book.NewBid(Order(2, 101, 20));  // total_bid = 30
    book.NewAsk(Order(3, 105, 5));   // total_ask = 5
    book.NewAsk(Order(4, 106, 8));   // total_ask = 13

    auto md_update = book.GenerateMDUpdate();
    REQUIRE(md_update.has_value());
    CHECK(md_update->total_bid_qty == 30);
    CHECK(md_update->total_ask_qty == 13);

    book.ReplaceBid(Order(2, 101, 20), Order(2, 102, 30));
    md_update = book.GenerateMDUpdate();
    REQUIRE(md_update.has_value());
    CHECK(md_update->total_bid_qty == 40);
    CHECK(md_update->total_ask_qty == 13);

    book.CancelAsk(3);              // total_ask = 8
    md_update = book.GenerateMDUpdate();
    REQUIRE(md_update.has_value());
    CHECK(md_update->total_bid_qty == 40);
    CHECK(md_update->total_ask_qty == 8);

    book.NewAsk(Order(5, 104, 12));  // total_ask = 20
    md_update = book.GenerateMDUpdate();
    REQUIRE(md_update.has_value());
    CHECK(md_update->total_bid_qty == 40);
    CHECK(md_update->total_ask_qty == 20);
    CHECK(md_update->best_ask_price == 104);
}

TEST_CASE("GenerateMDUpdate returns nullopt when bids/asks empty") {
    OrderBook book;

    auto md_update = book.GenerateMDUpdate();
    CHECK_FALSE(md_update.has_value());

    book.NewBid(Order(1, 100, 10));
    md_update = book.GenerateMDUpdate();
    CHECK(md_update.has_value());
    CHECK(md_update->total_bid_qty == 10);
    CHECK(md_update->total_ask_qty == 0);

    book.CancelBid(1);
    md_update = book.GenerateMDUpdate();
    CHECK_FALSE(md_update.has_value());

    book.NewAsk(Order(2, 105, 5));
    md_update = book.GenerateMDUpdate();
    CHECK(md_update.has_value());
    CHECK(md_update->total_bid_qty == 0);
    CHECK(md_update->total_ask_qty == 5);
}

TEST_CASE("GenerateMDUpdate total quantities with price info consistency") {
    OrderBook book;

    // Add Orders with diff prices
    book.NewBid(Order(1, 100, 10));
    book.NewBid(Order(2, 99, 15));
    book.NewBid(Order(3, 98, 20));

    book.NewAsk(Order(4, 101, 5));
    book.NewAsk(Order(5, 102, 8));
    book.NewAsk(Order(6, 103, 12));

    auto md_update = book.GenerateMDUpdate();
    REQUIRE(md_update.has_value());

    CHECK(md_update->total_bid_qty == 45);
    CHECK(md_update->total_ask_qty == 25);

    CHECK(md_update->best_bid_price == 100);
    CHECK(md_update->best_ask_price == 101);

    CHECK(md_update->best_bid_qty == 10);
    CHECK(md_update->best_ask_qty == 5);
}
