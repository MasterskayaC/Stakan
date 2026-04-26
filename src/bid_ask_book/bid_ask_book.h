#pragma once

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "bid_ask_interface.h"
#include "../snapshots_broadcaster/snapshot_source.h"
#include "logger.h"

namespace server {

/// Структура для результата поиска заявок по уровню цены.
struct PricesInfo {
    std::vector<common::ID> ids_{};
    common::Quantity quantity_ = 0;
};

class OrderBook {
public:
    explicit OrderBook(bool enable_snapshot_feed = false);
    ~OrderBook();

    // TODO: оставить публичными парные методы Bid/Ask, а общую логику
    // добавления, отмены и замены ордеров вынести в приватные методы.

    void NewBid(common::Order order);
    void NewAsk(common::Order order);
    void CancelBid(common::ID order_id);
    void CancelAsk(common::ID order_id);
    void ReplaceBid(common::Order old_order, common::Order new_order);
    void ReplaceAsk(common::Order old_order, common::Order new_order);

    PricesInfo GetPricesBidsInfo(common::Price price) const;
    PricesInfo GetPricesAsksInfo(common::Price price) const;

    [[nodiscard]] common::Snapshot GetTopSnapshot() const;

    [[nodiscard]] common::Order BestBid() const;
    [[nodiscard]] common::Order BestAsk() const;
    [[nodiscard]] std::optional<common::MDUpdate> GenerateMDUpdate() const;

private:
    void StartSnapshotFeed();
    void StopSnapshotFeed();
    void SnapshotFeedLoop();

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

    using PriceOnlyIndex = boost::multi_index::ordered_non_unique<
        boost::multi_index::member<common::Order, common::Price, &common::Order::price>>;

    using IdIndex =
        boost::multi_index::hashed_unique<boost::multi_index::member<common::Order, common::ID, &common::Order::id>>;

    template <typename Comparator>
    using OrderContainer = boost::multi_index::multi_index_container<
        common::Order,
        boost::multi_index::indexed_by<PriceIndex<Comparator>, IdIndex, PriceOnlyIndex>>;

    using BidContainer = OrderContainer<BidComparator>;
    using AskContainer = OrderContainer<AskComparator>;

    BidContainer bids_;
    AskContainer asks_;

    const std::unique_ptr<ISnapshotSource> snapshot_source_ = makeTmpSnapshotCreator();
    const bool snapshot_feed_enabled_ = false;
    std::atomic<bool> snapshot_feed_running_{false};
    std::thread snapshot_feed_thread_;
    std::mutex snapshot_feed_mutex_;
    std::condition_variable snapshot_feed_cv_;

    // Мьютексы для потокобезопасного доступа к контейнерам bid/ask.
    mutable std::shared_mutex bids_mutex_;
    mutable std::shared_mutex asks_mutex_;
};
}  // namespace server
