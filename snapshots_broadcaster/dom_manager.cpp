#include "dom_manager.h"

#include <boost/asio/post.hpp>
#include <iostream>
#include <optional>

#include "snapshot_source.h"

namespace server {

    DOMManager::DOMManager(boost::asio::io_context& io_context, std::unique_ptr<ISnapshotSource> snapshot_source)
                    : client_list_(makeClientList())
                    , snapshot_source_(std::move(snapshot_source))
                    , broadcaster_(std::make_unique<Broadcaster>(*client_list_, io_context))
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
                                        try {
                                            send_snapshot(use_test_broadcast);
                                            schedule_next_broadcast(interval_ms, use_test_broadcast);
                                        } catch (const std::exception& e) {
                                            std::cerr << "Error during scheduled broadcast: " << e.what() << std::endl;
                                            //stop_broadcasting();
                                        }

                                    }
        );
    }

    void DOMManager::send_snapshot_by_client_id(ClientId client_id, bool use_test_broadcast) {
        try {
            std::optional<common::Snapshot> snapshot = snapshot_source_->get_snapshot();
            if (!snapshot) {
                return;
            }
            auto cmd = std::make_unique<BroadcastSnapshotCommand>(client_id, std::move(*snapshot));
            broadcaster_->enqueue(std::move(cmd));
        } catch (const std::exception& e) {
            std::cerr << "Failed to send snapshot to client " << client_id << ": " << e.what() << std::endl;
            //stop_broadcasting();
        }
    }

    void DOMManager::send_snapshot(bool use_test_broadcast) {
        try {
            std::optional<common::Snapshot> snapshot = snapshot_source_->get_snapshot();
            if (!snapshot) {
                return;
            }
            if (!snapshot->topAsks.empty()) {       //TMP
                std::cout << "snapshot.topAsks[0].id = " << snapshot->topAsks[0].id << std::endl;
            }
            auto cmd = std::make_unique<BroadcastSnapshotCommand>(std::move(*snapshot));
            broadcaster_->enqueue(std::move(cmd));
        } catch (const std::exception& e) {
            std::cerr << "Failed to send snapshot: " << e.what() << std::endl;
            //stop_broadcasting();
        }
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
        send_snapshot_by_client_id(cmd.client_id, cmd.use_test_broadcast);
    }

    void DOMManager::Handle(const StartBroadcastCmd& cmd) {
        if (broadcasting_) return;
        start_broadcasting(cmd.interval_ms, cmd.use_test_broadcast);
    }

} // namespace server
