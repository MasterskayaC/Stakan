#pragma once

#include "snapshot_client.h"

#include <functional>

namespace console {

struct OrderLevel {
    uint64_t id = 0;
    uint64_t price = 0;
    int quantity = 0;
};

struct LowerLevelSnapshot {
    std::array<OrderLevel, 20> topBids;
    std::array<OrderLevel, 20> topAsks;
};

struct LowerLevelTopOfBook {
    std::string symbol;
    double bid_price{0.0};
    double bid_quantity{0.0};
    double ask_price{0.0};
    double ask_quantity{0.0};
    uint64_t timestamp{0};
};

class SnapshotAdapter {
public:
    using SnapshotCallback = std::function<void(const Snapshot&)>;
    using UpdateCallback = std::function<void(const TopOfBook&)>;

    explicit SnapshotAdapter(SnapshotCallback snapshot_cb, UpdateCallback update_cb)
        : snapshot_callback_(std::move(snapshot_cb))
        , update_callback_(std::move(update_cb)) {}

    void adapt_snapshot(const LowerLevelSnapshot& lower_snapshot) {
        Snapshot adapted;
        for (size_t i = 0; i < 20; ++i) {
            adapted.topBids[i].id = lower_snapshot.topBids[i].id;
            adapted.topBids[i].price = lower_snapshot.topBids[i].price;
            adapted.topBids[i].quantity = lower_snapshot.topBids[i].quantity;
            adapted.topAsks[i].id = lower_snapshot.topAsks[i].id;
            adapted.topAsks[i].price = lower_snapshot.topAsks[i].price;
            adapted.topAsks[i].quantity = lower_snapshot.topAsks[i].quantity;
        }
        if (snapshot_callback_) {
            snapshot_callback_(adapted);
        }
    }

    void adapt_top_of_book(const LowerLevelTopOfBook& lower_tob) {
        TopOfBook adapted;
        adapted.symbol = lower_tob.symbol;
        adapted.best_bid.price = lower_tob.bid_price;
        adapted.best_bid.quantity = lower_tob.bid_quantity;
        adapted.best_ask.price = lower_tob.ask_price;
        adapted.best_ask.quantity = lower_tob.ask_quantity;
        adapted.exchange_timestamp_ns = lower_tob.timestamp;
        if (update_callback_) {
            update_callback_(adapted);
        }
    }

private:
    SnapshotCallback snapshot_callback_;
    UpdateCallback update_callback_;
};

} // namespace console
