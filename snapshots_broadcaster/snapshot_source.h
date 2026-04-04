#pragma once

#include "bid_ask_interface.h"
#include <memory>
#include <optional>

class ISnapshotSource {
public:
	virtual ~ISnapshotSource() = default;
	virtual std::optional<common::Snapshot> get_snapshot() = 0;
};

/**
 * @brief Creates a new instance of ISnapshotSource.
 * @return A unique pointer owning the newly created ISnapshotSource object TmpSnapshotCreator.
 */
constexpr uint8_t TMP_NUM_OF_SNAPSHOTS = 5;
std::unique_ptr<ISnapshotSource> makeTmpSnapshotCreator(bool is_random = false, uint8_t snapshot_count = TMP_NUM_OF_SNAPSHOTS);
