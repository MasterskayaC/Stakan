#include "snapshot_source.h"

#include <random>

#include "order_book_snapshot_source.h"

/**
 * @brief Helper struct that generates and cycles through test snapshots
 */
class TmpSnapshotCreator : public ISnapshotSource {
public:
    /**
     * @brief Generates a vector of static (deterministic) snapshots
     * @param count Number of snapshots to generate
     * @return Vector of snapshots
    */
    static std::vector<common::Snapshot> generate_static_snapshots(int count);
    /**
     * @brief Generates a vector of random snapshots
     * @param count Number of snapshots to generate
     * @return Vector of snapshots
    */
    static std::vector<common::Snapshot> generate_random_snapshots(int count);

    /**
     * @brief Constructs a TmpSnapshotCreator
     * @param random If true, random snapshots are generated; otherwise static
     * @param count  Number of snapshots to generate
    */
    TmpSnapshotCreator(bool random, uint8_t count);

    /**
     * @brief Returns the next snapshot in the cyclic list
     * @return A snapshot
    */
    std::optional<common::Snapshot> get_snapshot() override;

private:
    std::vector<common::Snapshot> snapshots_;
    std::atomic<size_t> current_index_ = 0;
    bool random_;
};

// TmpSnapshotCreator implementation

std::vector<common::Snapshot> TmpSnapshotCreator::generate_static_snapshots(int count) {
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

std::vector<common::Snapshot> TmpSnapshotCreator::generate_random_snapshots(int count) {
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

TmpSnapshotCreator::TmpSnapshotCreator(bool random, uint8_t count)
    : random_(random) {
    if (random) {
        snapshots_ = generate_random_snapshots(count);
    } else {
        snapshots_ = generate_static_snapshots(count);
    }
}

std::optional<common::Snapshot> TmpSnapshotCreator::get_snapshot() {
    if (!snapshots_.size()) {
        return std::nullopt;
    }
    size_t idx = current_index_.fetch_add(1, std::memory_order_relaxed) % snapshots_.size();
    return snapshots_[idx];
}

std::unique_ptr<ISnapshotSource> makeTmpSnapshotCreator(bool is_random, uint8_t snapshot_count) {
    return std::make_unique<TmpSnapshotCreator>(is_random, snapshot_count);
}
std::unique_ptr<ISnapshotSource> makeOrderBookSnapshotSource(server::OrderBook* order_book) {
    return std::make_unique<OrderBookSnapshotSource>(order_book);
}