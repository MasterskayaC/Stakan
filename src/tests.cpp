#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "bid_ask_book.h"

using namespace server;

TEST_CASE("NewOrder adds to correct side") {
    OrderBook book;

    Order bid1{1, Side::Bid, 100, 10};
    Order ask1{2, Side::Ask, 105, 5};

    book.NewOrder(bid1);
    book.NewOrder(ask1);

    auto snapshot = book.GetTopSnapshot();
    REQUIRE(snapshot.topBids.size() == 1);
    REQUIRE(snapshot.topAsks.size() == 1);

    CHECK(snapshot.topBids[0]->id == 1);
    CHECK(snapshot.topAsks[0]->id == 2);
}

TEST_CASE("CancelOrder removes correctly") {
    OrderBook book;
    Order bid1{1, Side::Bid, 100, 10};
    book.NewOrder(bid1);

    book.CancelOrder(Side::Bid, 1);
    auto snapshot = book.GetTopSnapshot();
    CHECK(snapshot.topBids.empty());

    // cancelling non-existent id не ломает
    book.CancelOrder(Side::Ask, 42);
}

TEST_CASE("ReplaceOrder updates quantity and price") {
    OrderBook book;
    Order bid1{1, Side::Bid, 100, 10};
    book.NewOrder(bid1);

    Order new_order{2, Side::Ask, 20, 10};
    book.ReplaceOrder(bid1, new_order);

    auto snapshot = book.GetTopSnapshot();
    REQUIRE(snapshot.topBids.empty());
    REQUIRE(snapshot.topAsks.size() == 1);
    CHECK(snapshot.topAsks[0]->quantity == 10);
    CHECK(snapshot.topAsks[0]->price == 20);
}

TEST_CASE("TopSnapshot respects topN") {
    OrderBook book;
    for (int i = 0; i < 5; ++i) {
        book.NewOrder({uint64_t(i), Side::Bid, 100 + i, 10 + i});
        book.NewOrder({uint64_t(i + 10), Side::Ask, 105 + i, 5 + i});
    }

    auto snapshot = book.GetTopSnapshot(3);
    REQUIRE(snapshot.topBids.size() == 3);
    REQUIRE(snapshot.topAsks.size() == 3);

    // лучшая цена для bid должна быть максимальной
    CHECK(snapshot.topBids[0]->price == 104);
    // лучшая цена для ask должна быть минимальной
    CHECK(snapshot.topAsks[0]->price == 105);
}

TEST_CASE("BestBid and BestAsk") {
    OrderBook book;
    book.NewOrder({1, Side::Bid, 100, 10});
    book.NewOrder({2, Side::Bid, 105, 5});
    book.NewOrder({3, Side::Ask, 110, 20});
    book.NewOrder({4, Side::Ask, 108, 8});

    const Order* bestBid = book.BestBid();
    const Order* bestAsk = book.BestAsk();

    REQUIRE(bestBid != nullptr);
    REQUIRE(bestAsk != nullptr);

    CHECK(bestBid->price == 105);
    CHECK(bestAsk->price == 108);
}