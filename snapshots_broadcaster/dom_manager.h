#pragma once

#include <algorithm>
#include <atomic>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>
#include <optional>
#include <random>
#include <thread>
#include <variant>
#include <vector>

#include "bid_ask_book.h"
#include "bid_ask_interface.h"
#include "broadcaster.h"
#include "client_list.h"
#include "command_queue.h"
#include "snapshot_source.h"

namespace server {

    struct AddOrderCmd {
        ClientId client_id;
        bool is_bid = true;
        common::Order order;
    };
    struct RemoveOrderCmd {
        ClientId client_id;
        OrderId id;
        bool is_bid = true;
    };
    struct StartBroadcastCmd {
        int interval_ms;
        bool use_test_broadcast = true;
    };
    struct StopBroadcastCmd {};
    struct SendSnapshotToClientCmd {
        ClientId client_id;
        bool use_test_broadcast;
    };

    /// Variant type holding all possible commands that can be sent to DOMManager.
    using Command = std::variant<AddOrderCmd, RemoveOrderCmd, StartBroadcastCmd, StopBroadcastCmd, SendSnapshotToClientCmd>;

    /**
     * @brief Manages the Depth of Market (DOM) and broadcasts snapshots to clients
     */
    class DOMManager {
    public:
        static constexpr int DEFAULT_INTERVAL = 200;    ///< Default broadcast interval in milliseconds

        /**
         * @brief Constructs a DOMManager instance
         * @param io_context The Boost.Asio io_context used for asynchronous operations
         */
        explicit DOMManager(boost::asio::io_context& io_context);

        /**
         * @brief Destroys the DOMManager, stops broadcasting and cleans up resources
         */
        ~DOMManager();

        /**
         * @brief Processes a command by posting it to the io_context for execution
         * @param cmd The command to process
         */
        void operator()(Command cmd);

    private:
        std::unique_ptr<IClientList> client_list_;           ///< connected clients
        std::unique_ptr<ISnapshotSource> snapshot_source_;   ///< Source of snapshots (order book or test generator)
        std::unique_ptr<Broadcaster> broadcaster_;           ///< Component responsible for sending data to clients

        std::atomic<bool> broadcasting_{ false };            ///< Flag indicating whether broadcasting is active
        boost::asio::steady_timer broadcast_timer_;          ///< Timer used for periodic broadcasts

        boost::asio::io_context& io_context_;                ///< Reference to the io_context for asynchronous operations

        /**
         * @brief Starts broadcasting snapshots at regular intervals
         * @param interval_ms Time in milliseconds between snapshots (default = DEFAULT_INTERVAL)
         * @param use_test_broadcast If true, uses test snapshots from TmpSnapshotCreator;
         *                           otherwise uses the order book snapshots
         */
        void start_broadcasting(int interval_ms = DEFAULT_INTERVAL, bool use_test_broadcast = true);

        /**
         * @brief Stops broadcasting snapshots and cancels the timer
         */
        void stop_broadcasting();

        /**
         * @brief Sends the current snapshot to all clients via the broadcaster
         * @param use_test_broadcast If true, uses test snapshots from TmpSnapshotCreator;
         *                           otherwise uses the order book snapshots
         */
        void send_snapshot(bool use_test_broadcast = false);

        /**
         * @brief Sends a snapshot to a specific client
         * @param client_id Identifier of the target client
         * @param use_test_broadcast If true, uses a test snapshot from TmpSnapshotCreator
         *                           otherwise uses the current snapshot from the order book
         */
        void send_snapshot_by_client_id(ClientId client_id, bool use_test_broadcast = true);

        /**
         * @brief Schedules the next broadcast by restarting the timer
         * @param interval_ms The interval for the next broadcast
         * @param use_test_broadcast Whether to use test snapshots
         * @todo Consider moving periodic broadcast logic to Broadcaster class
        */
        void schedule_next_broadcast(int interval_ms, bool use_test_broadcast); ///< Maybe it'll move to a broadcaster

        // Command handlers (called from operator() after dispatching to io_context)
        void Handle(const AddOrderCmd& cmd);               ///< Handles AddOrderCmd
        void Handle(const RemoveOrderCmd& cmd);            ///< Handles RemoveOrderCmd
        void Handle(const StopBroadcastCmd&);              ///< Handles StopBroadcastCmd
        void Handle(const SendSnapshotToClientCmd& cmd);   ///< Handles SendSnapshotToClientCmd
        void Handle(const StartBroadcastCmd& cmd);         ///< Handles StartBroadcastCmd
    };

} // namespace server
