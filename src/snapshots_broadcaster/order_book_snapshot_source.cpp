#include "order_book_snapshot_source.h"
#include <random>


OrderBookSnapshotSource::OrderBookSnapshotSource(server::OrderBook *order_book) : order_book_(order_book) {
}

int64_t OrderBookSnapshotSource::GetNewBid() {
    return rg_.GetRandom();
}

int64_t OrderBookSnapshotSource::GetNewAsk() {
    return rg_.GetRandom();
}

std::optional<common::Snapshot> OrderBookSnapshotSource::get_snapshot() {
    return order_book_->GetTopSnapshot();
}


