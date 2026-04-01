#pragma once

#include "bid_ask_interface.h"
#include <memory>

class ISnapshotSource {
public:
	virtual ~ISnapshotSource() = default;
	virtual common::Snapshot get_snapshot() = 0;
};

/**
 * @brief Creates a new instance of ISnapshotSource.
 * @return A unique pointer owning the newly created ISnapshotSource object TmpSnapshotCreator.
 */
std::unique_ptr<ISnapshotSource> makeTmpSnapshotCreator(bool is_random = false, uint8_t snapshot_count = 5);
