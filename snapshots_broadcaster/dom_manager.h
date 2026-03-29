#pragma once

#include <memory>
#include <optional>
#include <thread>
#include <atomic>
#include <vector>
#include <random>
#include <algorithm>

#include "bid_ask_book.h"
#include "broadcaster.h"
#include "client_list.h"
#include "command_queue.h"
#include "bid_ask_interface.h"

namespace server {

    /**
     * @brief Manages the Depth of Market (DOM) and broadcasts snapshots to clients
     */
    class DOMManager {
    public:
        /**
         * @brief Constructs a DOMManager instance
         */
        DOMManager();

        /**
         * @brief Destroys the DOMManager, stops broadcasting
         */
        ~DOMManager();

        /**
         * @brief Starts broadcasting snapshots at regular intervals
         * @param interval_ms Time in milliseconds between snapshots (default = 200)
         * @param use_test_broadcast If true, uses a test snapshots from TmpSnapshotCreator,
         *                          otherwise uses the order book snapshots
         */
        void start_broadcasting(int interval_ms = 200, bool use_test_broadcast = true);

        /**
         * @brief Stops broadcasting snapshots and waits for the broadcasting thread to finish
         */
        void stop_broadcasting();

        /**
         * @brief Sends a snapshot to a specific client
         * @param client_id Identifier of the target client
         * @param use_test_broadcast If true, uses a test snapshot from TmpSnapshotCreator,
         *                          otherwise uses the current snapshot from the order book
         */
        void send_snapshot_to_client(ClientId client_id, bool use_test_broadcast = true);

    private:
        /**
         * @brief Helper struct that generates and cycles through test snapshots
         */
        struct TmpSnapshotCreator {
            std::vector<common::Snapshot> snapshots_;
            size_t current_index_ = 0;
            bool random_;

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
            common::Snapshot next();
        };

        std::unique_ptr<IClientList> client_list_;
        std::unique_ptr<OrderBook>   order_book_;
        std::unique_ptr<Broadcaster> broadcaster_;
        TmpSnapshotCreator tmp_snapshot_creator_;

        std::thread         broadcaster_thread_;
        std::atomic<bool>   broadcasting_{ false };

        /**
         * @brief Sends the current snapshot to all clients via the broadcaster
         * @param use_test_broadcast If true, uses a test snapshots from TmpSnapshotCreator,
         *                          otherwise uses the order book snapshots
         */
        void send_snapshots(bool use_test_broadcast = false);
    };

} // namespace server
