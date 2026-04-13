#pragma once

#include <chrono>
#include <shared_mutex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include "bid_ask_snaphot_interface/bid_ask_interface.h"
#include "snapshots_broadcaster/snapshot_source.h"
#include "logger.h"

namespace server {
class OrderBook {
public:
    // TODO: Оставить публичными парные методы Bid/Ask, а общую логику добавления,
    // отмены и замены ордеров вынести в приватные методы

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
            return a.price > b.price || (a.price == b.price && a.quantity > b.quantity) ||
                   (a.price == b.price && a.quantity == b.quantity && a.id < b.id);
        }
    };

    struct AskComparator {
        bool operator()(const common::Order& a, const common::Order& b) const {
            return a.price < b.price || (a.price == b.price && a.quantity > b.quantity) ||
                   (a.price == b.price && a.quantity == b.quantity && a.id < b.id);
        }
    };

    template <typename Comparator>
    using PriceIndex = boost::multi_index::ordered_non_unique<boost::multi_index::identity<common::Order>, Comparator>;

    using IdIndex =
        boost::multi_index::hashed_unique<boost::multi_index::member<common::Order, common::ID, &common::Order::id>>;

    template <typename Comparator>
    using OrderContainer =
        boost::multi_index::multi_index_container<common::Order,
                                                  boost::multi_index::indexed_by<PriceIndex<Comparator>, IdIndex>>;

    using BidContainer = OrderContainer<BidComparator>;
    using AskContainer = OrderContainer<AskComparator>;

    BidContainer bids_;
    AskContainer asks_;

  
    const std::unique_ptr<ISnapshotSource> snapshot_source_ = makeTmpSnapshotCreator();

    // мьютексы для потокобезопасности
    mutable std::shared_mutex bids_mutex_;
    mutable std::shared_mutex asks_mutex_;
    };
}
