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

    CHECK(snapshot->topBids[0].id == 1);
    CHECK(snapshot->topBids[0].price == 100);

    CHECK(snapshot->topAsks[0].id == 2);
    CHECK(snapshot->topAsks[0].price == 105);
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
    CHECK(snapshot->topBids[0].id == 0);
    CHECK(snapshot->topAsks[0].id == 0);
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

    CHECK(snapshot->topBids[0].price == 110);
    CHECK(snapshot->topBids[0].quantity == 15);

    CHECK(snapshot->topAsks[0].price == 102);
    CHECK(snapshot->topAsks[0].quantity == 7);
}

TEST_CASE("BestBid and BestAsk return correct orders") {
    OrderBook book;

    book.NewBid(Order(1, 100, 10));
    book.NewBid(Order(2, 105, 5));
    book.NewAsk(Order(3, 110, 20));
    book.NewAsk(Order(4, 108, 8));

    Order bestBid = book.BestBid();
    Order bestAsk = book.BestAsk();

    CHECK(bestBid.price == 105); // max price
    CHECK(bestAsk.price == 108); // min price
}


TEST_CASE("Ordering works correctly for bids and asks") {
    OrderBook book;

    // одинаковые цены → проверяем quantity и id
    book.NewBid(Order(1, 100, 10));
    book.NewBid(Order(2, 100, 20)); // должен быть выше (quantity больше)

    book.NewAsk(Order(3, 105, 10));
    book.NewAsk(Order(4, 103, 10)); // должен быть выше (цена меньше)

    auto snapshot = book.GetTopSnapshot();

    CHECK(snapshot->topBids[0].id == 2); // больше quantity
    CHECK(snapshot->topAsks[0].id == 4); // меньше price
}

TEST_CASE("Replace with different id does nothing") {
    OrderBook book;

    Order bid1(1, 100, 10);
    book.NewBid(bid1);

    Order invalid(2, 110, 15);
    book.ReplaceBid(bid1, invalid);

    auto snapshot = book.GetTopSnapshot();

    // ордер не изменился
    CHECK(snapshot->topBids[0].id == 1);
    CHECK(snapshot->topBids[0].price == 100);
}

TEST_CASE ("Check bestBid function") {
    SECTION("Empty orderbook") {
        OrderBook book;
        Order empty_order;
        const Order o = book.BestBid();
        REQUIRE(o == empty_order);
    }

    SECTION("Non empty orderbook") {
        OrderBook book;
        book.NewBid(Order(1, 100, 10));
        book.NewBid(Order(2, 100, 20));

        Order bid1{2, 100, 20};
        const Order best_bid1 = book.BestBid();
        REQUIRE(bid1 == best_bid1);

        Order bid2{3, 200, 20}; ///новый "лучший" бид
        book.NewBid(Order(3, 200, 20));
        const Order best_bid2 = book.BestBid();
        REQUIRE(bid2 == best_bid2);
    }
}

TEST_CASE ("Check bestAsk function") {
    SECTION("Empty orderbook") {
        OrderBook book;
        Order empty_order;
        const Order o = book.BestAsk();
        REQUIRE(o == empty_order);
    }

    SECTION("Non empty orderbook") {
        OrderBook book;
        book.NewAsk(Order(1, 100, 10));
        book.NewAsk(Order(2, 100, 20));

        Order ask1{2, 100, 20};
        const Order best_ask1 = book.BestAsk();
        REQUIRE(ask1 == best_ask1);

        Order ask2{3, 50, 20}; ///новый "лучший" аск
        book.NewAsk(Order(3, 50, 20));
        const Order best_ask2 = book.BestAsk();
        REQUIRE(ask2 == best_ask2);
    }
}
