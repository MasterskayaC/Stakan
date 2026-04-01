#include "dom_manager.h"

#include <boost/asio/post.hpp>
#include <iostream>

namespace server {
    constexpr uint8_t TMP_NUM_OF_SNAPSHOTS = 5;

    DOMManager::DOMManager(boost::asio::io_context& io_context)
                    : client_list_(makeClientList())
                    , snapshot_source_(makeTmpSnapshotCreator(false, TMP_NUM_OF_SNAPSHOTS))
                    , broadcaster_(std::make_unique<Broadcaster>(*client_list_))
                    , broadcast_timer_(io_context)
                    , io_context_(io_context) {
        broadcaster_->start();
    }

    DOMManager::~DOMManager() {
        stop_broadcasting();
        if (broadcaster_) {
            broadcaster_->stop();
        }
    }

    void DOMManager::operator()(Command cmd) {
        boost::asio::post(io_context_, [this, cmd = std::move(cmd)]() mutable {
            std::visit([this](auto&& arg) { Handle(arg); }, cmd);
        });
    }

    void DOMManager::start_broadcasting(int interval_ms, bool use_test_broadcast) {
        if (broadcasting_) return;
        broadcasting_ = true;
        schedule_next_broadcast(interval_ms, use_test_broadcast);
    }

    void DOMManager::stop_broadcasting() {
        broadcasting_ = false;
        broadcast_timer_.cancel();
    }

    void DOMManager::schedule_next_broadcast(int interval_ms, bool use_test_broadcast) {
        if (!broadcasting_) return;
        broadcast_timer_.expires_after(std::chrono::milliseconds(interval_ms));
        broadcast_timer_.async_wait([this, interval_ms, use_test_broadcast](const boost::system::error_code& ec) {
                                        if (ec || !broadcasting_) {
                                            return;
                                        }
                                        send_snapshots(use_test_broadcast);
                                        schedule_next_broadcast(interval_ms, use_test_broadcast);
                                    }
        );
    }

    void DOMManager::send_snapshot_to_client(ClientId client_id, bool use_test_broadcast) {
        common::Snapshot snapshot = snapshot_source_->get_snapshot();
        auto cmd = std::make_unique<BroadcastSnapshotCommand>(client_id, std::move(snapshot));
        broadcaster_->enqueue(std::move(cmd));
    }

    void DOMManager::send_snapshots(bool use_test_broadcast) {
        common::Snapshot snapshot = snapshot_source_->get_snapshot();
        std::cout << snapshot.topAsks[0].id << std::endl;
        auto cmd = std::make_unique<BroadcastSnapshotCommand>(std::move(snapshot));
        broadcaster_->enqueue(std::move(cmd));
    }

    void DOMManager::Handle(const AddOrderCmd& cmd) {
        return;
    }
    void DOMManager::Handle(const RemoveOrderCmd& cmd) {
        return;
    }

    void DOMManager::Handle(const StopBroadcastCmd&) {
        stop_broadcasting();
    }
    void DOMManager::Handle(const SendSnapshotToClientCmd& cmd) {
        send_snapshot_to_client(cmd.client_id, cmd.use_test_broadcast);
    }

    void DOMManager::Handle(const StartBroadcastCmd& cmd) {
        if (broadcasting_) return;
        start_broadcasting(cmd.interval_ms, cmd.use_test_broadcast);
    }

} // namespace server
