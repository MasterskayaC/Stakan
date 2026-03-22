#pragma once

#include <vector>
#include <chrono>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>


namespace server {
    enum class LogLevel {
        Info,
        Warning,
        Error
    };

    class Logger {
    public:
        static void Log(LogLevel level, const std::string& msg);

    private:
        static std::string Now();
        static const char* ToString(LogLevel level);
    };


    using ID = uint64_t;
    using Price = int64_t;

    enum class Side {Bid, Ask};

    std::string SideToString (Side side);

    // предполагается, что будем брать из Interface
    struct Order {
        ID id = 0;
        Side side = Side::Ask;
        Price price = 0;
        int quantity = 0;
    };

    class OrderBook {
    public:
        void NewOrder(const Order& order);
        void CancelOrder(Side side, ID order_id);
        void ReplaceOrder(const Order& old_order, const Order& new_order);

        struct Snapshot {
            std::vector<const Order*> topBids;
            std::vector<const Order*> topAsks;
        };

        [[nodiscard]] Snapshot GetTopSnapshot(size_t topN = 20) const;

        [[nodiscard]] const Order* BestBid() const;
        [[nodiscard]] const Order* BestAsk() const;

    private:
        struct BidComparator {
            bool operator()(const Order& a, const Order& b) const {
                if (a.price != b.price) return a.price > b.price;
                if (a.quantity != b.quantity) return a.quantity > b.quantity;
                return a.id < b.id;
            }
        };
        struct AskComparator {
            bool operator()(const Order& a, const Order& b) const {
                if (a.price != b.price) return a.price < b.price;
                if (a.quantity != b.quantity) return a.quantity > b.quantity;
                return a.id < b.id;
            }
        };

        // то же что set (сортировка по цене) + map <Id, it>, только атомарный + надежнее
        using BidContainer = boost::multi_index::multi_index_container<
            Order,
            boost::multi_index::indexed_by<
                // индекс по цене
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::identity<Order>,
                    BidComparator
                >,
                // индекс по id
                boost::multi_index::hashed_unique<
                    boost::multi_index::member<Order, ID, &Order::id>
                >
            >
        >;

        using AskContainer = boost::multi_index::multi_index_container<
            Order,
            boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::identity<Order>,
                    AskComparator
                >,
                boost::multi_index::hashed_unique<
                    boost::multi_index::member<Order, ID, &Order::id>
                >
            >
        >;

        BidContainer bids_;
        AskContainer asks_;
    };
}