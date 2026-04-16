#include "order_book_snapshot_source.h"
#include<random>


OrderBookSnapshotSource::OrderBookSnapshotSource(server::OrderBook *order_book) : order_book_(order_book) {
}

double OrderBookSnapshotSource::GetNewBid() {
    return rg.GetRandom();
}

double OrderBookSnapshotSource::GetNewAsk() {
    return rg.GetRandom();
}

std::optional<common::Snapshot> OrderBookSnapshotSource::get_snapshot() {
    return order_book_->GetTopSnapshot();
}

RandomGenerator::RandomGenerator(): gen(rd()), dist(0, 9999){
         } 

double RandomGenerator::GetRandom() {
        return dist(gen);
    }


