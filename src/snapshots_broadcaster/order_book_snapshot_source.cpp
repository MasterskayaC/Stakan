#include "order_book_snapshot_source.h"
#include <random>


OrderBookSnapshotSource::OrderBookSnapshotSource(server::OrderBook *order_book) : order_book_(order_book) {
}

int64_t OrderBookSnapshotSource::GetNewBid() {
    return static_cast<int64_t>(order_book_->BestBid().get_price());
}

int64_t OrderBookSnapshotSource::GetNewAsk() {
    return static_cast<int64_t>(order_book_->BestAsk().get_price());
}

std::optional<common::Snapshot> OrderBookSnapshotSource::get_snapshot() {
    return order_book_->GetTopSnapshot();
}


