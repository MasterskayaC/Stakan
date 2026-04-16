#pragma once

#include "bid_ask_interface.h"
#include <memory>
#include <optional>
#include<random>
#include"../bid_ask_book/src/bid_ask_book.h"

namespace server {
    class OrderBook;  // Если OrderBook действительно в namespace server
}

class RandomGenerator{
     std::random_device rd;
     std::mt19937 gen;
     std::uniform_real_distribution<double> dist;
    public:
         RandomGenerator();
         double GetRandom();
};


class ISnapshotSource {
public:
    virtual ~ISnapshotSource() = default;

    virtual std::optional<common::Snapshot> get_snapshot() = 0;
    virtual double GetNewBid() = 0;
    virtual double GetNewAsk() = 0;
};

/**
 * @brief Creates a new instance of ISnapshotSource.
 * @return A unique pointer owning the newly created ISnapshotSource object TmpSnapshotCreator.
 */
std::unique_ptr<ISnapshotSource> makeTmpSnapshotCreator(bool is_random = false, uint8_t snapshot_count = 5);

/**
 * @brief Creates a new instance of ISnapshotSource.
 * @return A unique pointer owning the newly created ISnapshotSource object OrderBookSnapshotSource
 * that asks OrderBook for snapshots directly.
 */

std::unique_ptr<ISnapshotSource> makeOrderBookSnapshotSource(server::OrderBook* order_book);
