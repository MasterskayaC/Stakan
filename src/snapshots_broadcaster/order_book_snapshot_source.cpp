#include "order_book_snapshot_source.h"
#include <random>
#include "../bid_ask_book/bid_ask_book.h"

OrderBookSnapshotSource::OrderBookSnapshotSource(server::OrderBook *order_book) : order_book_(order_book) {
}

common::Order OrderBookSnapshotSource::GetNewBid() {
    return order_book_->BestBid();
}

common::Order OrderBookSnapshotSource::GetNewAsk() {
    return order_book_->BestAsk();
}

std::optional<common::Snapshot> OrderBookSnapshotSource::get_snapshot() {
    return order_book_->GetTopSnapshot();
}


