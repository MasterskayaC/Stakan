#include "dom_manager.h"

#include <iostream>

namespace server {

    constexpr uint8_t TMP_NUM_OF_SNAPSHOTS = 5;

    DOMManager::DOMManager()
        : client_list_(makeClientList())
        , order_book_(std::make_unique<OrderBook>())
        , broadcaster_(std::make_unique<Broadcaster>(*client_list_))
        , tmp_snapshot_creator_(false, TMP_NUM_OF_SNAPSHOTS) {
        broadcaster_->start();
    }

    DOMManager::~DOMManager() {
        stop_broadcasting();
        if (broadcaster_) {
            broadcaster_->stop();
        }
    }

    void DOMManager::start_broadcasting(int interval_ms, bool use_test_broadcast) {
        if (broadcasting_) return;
        broadcasting_ = true;
        broadcaster_thread_ = std::thread([this, interval_ms, use_test_broadcast]() {
            while (broadcasting_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                send_snapshots(use_test_broadcast);
            }
            });
    }

    void DOMManager::stop_broadcasting() {
        broadcasting_ = false;
        if (broadcaster_thread_.joinable()) {
            broadcaster_thread_.join();
        }
    }

    void DOMManager::send_snapshot_to_client(ClientId client_id, bool use_test_broadcast) {
        common::Snapshot snapshot;
        if (use_test_broadcast) {
            if (tmp_snapshot_creator_.snapshots_.empty()) return;
            snapshot = tmp_snapshot_creator_.next();
        } else {
            snapshot = order_book_->GetTopSnapshot();
        }
        BroadcastCommand cmd{ CommandType::SendSnapshot, client_id, snapshot };
        broadcaster_->enqueue(cmd);
    }

    void DOMManager::send_snapshots(bool use_test_broadcast) {
        common::Snapshot snapshot;
        if (use_test_broadcast) {
            if (tmp_snapshot_creator_.snapshots_.empty()) return;
            snapshot = tmp_snapshot_creator_.next();
        } else {
            snapshot = order_book_->GetTopSnapshot();
        }
        BroadcastCommand cmd{ CommandType::SendSnapshot, std::nullopt, snapshot };
        broadcaster_->enqueue(cmd);
    }

    // TmpSnapshotCreator implementation

    std::vector<common::Snapshot> DOMManager::TmpSnapshotCreator::generate_static_snapshots(int count) {
        std::vector<common::Snapshot> snapshots;
        snapshots.resize(count);
        for (size_t i = 0; i < count; ++i) {
            auto& snap = snapshots[i];
            for (size_t j = 0; j < common::topN; ++j) {
                snap.topBids[j].id = i * 100 + j;
                snap.topBids[j].price = 100 - i * 5 - j * 1;
                snap.topBids[j].quantity = 1000 + i * 100 + j * 10;
                snap.topAsks[j].id = i * 100 + j + 1000;
                snap.topAsks[j].price = 101 + i * 5 + j * 1;
                snap.topAsks[j].quantity = 900 + i * 100 + j * 10;
            }
        }
        return snapshots;
    }

    std::vector<common::Snapshot> DOMManager::TmpSnapshotCreator::generate_random_snapshots(int count) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> price_dist(50, 100);
        std::uniform_int_distribution<common::Price> qty_dist(500, 1000);
        std::uniform_int_distribution<common::ID> id_dist(1, 10000);

        std::vector<common::Snapshot> snapshots;
        snapshots.resize(count);
        for (size_t i = 0; i < count; ++i) {
            auto& snap = snapshots[i];
            for (size_t j = 0; j < common::topN; ++j) {
                snap.topBids[j].id = id_dist(gen);
                snap.topBids[j].price = price_dist(gen);
                snap.topBids[j].quantity = qty_dist(gen);
                snap.topAsks[j].id = id_dist(gen);
                snap.topAsks[j].price = price_dist(gen);
                snap.topAsks[j].quantity = qty_dist(gen);
            }
            std::sort(snap.topBids.begin(), snap.topBids.end(),
                [](const common::Order& a, const common::Order& b) { return a.price > b.price; });
            std::sort(snap.topAsks.begin(), snap.topAsks.end(),
                [](const common::Order& a, const common::Order& b) { return a.price < b.price; });
        }
        return snapshots;
    }

    DOMManager::TmpSnapshotCreator::TmpSnapshotCreator(bool random, uint8_t count)
        : random_(random) {
        if (random) {
            snapshots_ = generate_random_snapshots(count);
        } else {
            snapshots_ = generate_static_snapshots(count);
        }
    }

    common::Snapshot DOMManager::TmpSnapshotCreator::next() {
        common::Snapshot snap = snapshots_[current_index_];
        current_index_ = (current_index_ + 1) % snapshots_.size();
        return snap;
    }

} // namespace server
