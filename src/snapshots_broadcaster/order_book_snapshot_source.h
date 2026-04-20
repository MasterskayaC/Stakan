#pragma once
#include "snapshot_source.h"

#include "../bid_ask_book/src/bid_ask_book.h"
#include "../bid_ask_snaphot_interface/bid_ask_interface.h"


class OrderBookSnapshotSource : public ISnapshotSource {
    public:
    //в реализацию передается сырой указатель, однако лучше поменять на умный,
    //в зависимости от принципов владения OrderBookом
    OrderBookSnapshotSource(server::OrderBook* order_book);

    ~OrderBookSnapshotSource() override = default;

    std::optional<common::Snapshot> get_snapshot() override;
private:
    server::OrderBook *order_book_;
};

