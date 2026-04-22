#pragma once
#include <random>
#include "snapshot_source.h"

#include "../bid_ask_snaphot_interface/bid_ask_interface.h"



class OrderBookSnapshotSource : public ISnapshotSource {
    public:
    //в реализацию передается сырой указатель, однако лучше поменять на умный,
    //в зависимости от принципов владения OrderBookом
    OrderBookSnapshotSource(server::OrderBook* order_book);
    common::Order GetNewBid() override;
    common::Order GetNewAsk() override;

    ~OrderBookSnapshotSource() override = default;

    std::optional<common::Snapshot> get_snapshot() override;
private:
    RandomGenerator rg_;
    server::OrderBook *order_book_;
};

