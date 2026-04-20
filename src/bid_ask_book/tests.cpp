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
