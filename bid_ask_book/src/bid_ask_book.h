#pragma once

#include <chrono>
#include <shared_mutex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include "bid_ask_interface.h"
#include "logger.h"

namespace server {
    // первоначальная имплементация базового логгера, ToDo - перенести его в более подходящее место
    class OrderBook {
    public:
        //отдельный метод для каждого контейнера
        void NewBid(common::Order order);
        void NewAsk(common::Order order);
        void CancelBid(common::ID order_id);
        void CancelAsk(common::ID order_id);
        void ReplaceBid(common::Order old_order, common::Order new_order);
        void ReplaceAsk(common::Order old_order, common::Order new_order);

        [[nodiscard]] common::Snapshot GetTopSnapshot() const;

        [[nodiscard]] common::Order BestBid() const;
        [[nodiscard]] common::Order BestAsk() const;

    private:
        struct BidComparator {
            bool operator()(const common::Order& a, const common::Order& b) const {
                if (a.price != b.price) return a.price > b.price;
                if (a.quantity != b.quantity) return a.quantity > b.quantity;
                return a.id < b.id;
            }
        };
        struct AskComparator {
            bool operator()(const common::Order& a, const common::Order& b) const {
                if (a.price != b.price) return a.price < b.price;
                if (a.quantity != b.quantity) return a.quantity > b.quantity;
                return a.id < b.id;
            }
        };

        // то же что set (сортировка по цене) + map <Id, it>, только атомарный + надежнее
        using BidContainer = boost::multi_index::multi_index_container<
            common::Order,
            boost::multi_index::indexed_by<
                // индекс по цене
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::identity<common::Order>,
                    BidComparator
                >,
                // индекс по id
                boost::multi_index::hashed_unique<
                    boost::multi_index::member<common::Order, common::ID, &common::Order::id>
                >
            >
        >;

        using AskContainer = boost::multi_index::multi_index_container<
            common::Order,
            boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::identity<common::Order>,
                    AskComparator
                >,
                boost::multi_index::hashed_unique<
                    boost::multi_index::member<common::Order, common::ID, &common::Order::id>
                >
            >
        >;

        BidContainer bids_;
        AskContainer asks_;

        // мьютексы для потокобезопасности
        mutable std::shared_mutex bids_mutex_;
        mutable std::shared_mutex asks_mutex_;
    };
}
