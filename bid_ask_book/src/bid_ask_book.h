#pragma once

#include <mutex>
#include <optional>
#include <shared_mutex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include "bid_ask_interface.h"
#include "logger.h"

namespace server {
    class OrderBook {
    public:
        // отдельный метод для каждого контейнера
        void NewBid(common::Order order);
        void NewAsk(common::Order order);
        void CancelBid(common::ID order_id);
        void CancelAsk(common::ID order_id);
        void ReplaceBid(common::Order old_order, common::Order new_order);
        void ReplaceAsk(common::Order old_order, common::Order new_order);

        // снапшот кэшируется и инвалидируется только когда успешная операция меняет topN
        [[nodiscard]] common::Snapshot GetTopSnapshot() const;

        [[nodiscard]] common::Order BestBid() const;
        [[nodiscard]] common::Order BestAsk() const;

    private:
        struct BidComparator {
            bool operator()(const common::Order& a, const common::Order& b) const {
                return a.price > b.price
                    || (a.price == b.price && a.quantity > b.quantity)
                    || (a.price == b.price && a.quantity == b.quantity && a.id < b.id);
            }
        };

        struct AskComparator {
            bool operator()(const common::Order& a, const common::Order& b) const {
                return a.price < b.price
                    || (a.price == b.price && a.quantity > b.quantity)
                    || (a.price == b.price && a.quantity == b.quantity && a.id < b.id);
            }
        };

        template <typename Comparator>
        using PriceIndex = boost::multi_index::ordered_non_unique<
            boost::multi_index::identity<common::Order>,
            Comparator
        >;

        using IdIndex = boost::multi_index::hashed_unique<
            boost::multi_index::member<common::Order, common::ID, &common::Order::id>
        >;

        template <typename Comparator>
        // то же что set (сортировка по цене) + map <Id, it>, только атомарный + надежнее
        using OrderContainer = boost::multi_index::multi_index_container<
            common::Order,
            boost::multi_index::indexed_by<
                PriceIndex<Comparator>,
                IdIndex
            >
        >;

        using BidContainer = OrderContainer<BidComparator>;
        using AskContainer = OrderContainer<AskComparator>;

        template <typename Index>
        [[nodiscard]] static bool IsInTopN(const Index& index, common::ID order_id, size_t top_n);

        void ResetCachedSnapshot() const;
        [[nodiscard]] common::Snapshot BuildNewSnapshot() const;

        BidContainer bids_;
        AskContainer asks_;
        mutable std::optional<common::Snapshot> cached_snapshot_;

        // мьютексы для потокобезопасности
        mutable std::mutex snapshot_mutex_;
        mutable std::shared_mutex bids_mutex_;
        mutable std::shared_mutex asks_mutex_;
    };
}
