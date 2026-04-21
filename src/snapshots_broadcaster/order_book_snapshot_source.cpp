#include "order_book_snapshot_source.h"



OrderBookSnapshotSource::OrderBookSnapshotSource(server::OrderBook *order_book) : order_book_(order_book) {
}

std::optional<common::Snapshot> OrderBookSnapshotSource::get_snapshot() {
    return order_book_->GetTopSnapshot();
}

