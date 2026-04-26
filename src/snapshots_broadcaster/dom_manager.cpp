#include "dom_manager.h"

#include <boost/asio/post.hpp>
#include <iostream>
#include <optional>

namespace server {

DOMManager::DOMManager(boost::asio::io_context& io_context,
                       IClientList& client_list,
                       std::unique_ptr<OrderBook> order_book) :
    client_list_(client_list), order_book_(std::move(order_book)),
    broadcaster_(std::make_unique<Broadcaster>(client_list_, io_context)), broadcast_timer_(io_context),
    io_context_(io_context) {
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
        std::visit(
            [this](auto&& arg) {
                Handle(arg);
            },
            cmd);
    });
}


void DOMManager::start_broadcasting(int interval_ms, bool use_test_broadcast) {
    if (broadcasting_) {
        return;
    }
    broadcasting_ = true;
    schedule_next_broadcast(interval_ms, use_test_broadcast);
}

void DOMManager::stop_broadcasting() {
    broadcasting_ = false;
    broadcast_timer_.cancel();
}

void DOMManager::schedule_next_broadcast(int interval_ms, bool use_test_broadcast) {
    if (!broadcasting_) {
        return;
    }

    broadcast_timer_.expires_after(std::chrono::milliseconds(interval_ms));
    broadcast_timer_.async_wait(
        [this, interval_ms, use_test_broadcast](const boost::system::error_code& ec) {
            if (ec || !broadcasting_) {
                return;
            }

            try {
                // MDUpdate — каждый тик
                send_mdupdate(use_test_broadcast);

                // Snapshot — редко
                if (++tick_ % SNAPSHOT_EVERY_N_TICKS == 0) {
                    send_snapshot(use_test_broadcast);
                }

                schedule_next_broadcast(interval_ms, use_test_broadcast);
            } catch (const std::exception& e) {
                std::cerr << "Error during scheduled broadcast: " << e.what() << std::endl;
            }
        });
}

void DOMManager::send_snapshot_by_client_id(ClientId client_id, bool use_test_broadcast) {
    try {
        std::optional<common::Snapshot> snapshot = order_book_->GetTopSnapshot();
        if (!snapshot) {
            std::cout << "there is no snapshot" << std::endl;
            return;
        }
        auto cmd = std::make_unique<BroadcastSnapshotCommand>(client_id, std::move(*snapshot));
        broadcaster_->enqueue(std::move(cmd));
    } catch (const std::exception& e) {
        std::cerr << "Failed to send snapshot to client " << client_id << ": " << e.what() << std::endl;
        // stop_broadcasting();
    }
}


void DOMManager::send_snapshot(bool use_test_broadcast) {
    try {
        std::optional<common::Snapshot> snapshot = order_book_->GetTopSnapshot();
        if (!snapshot) {
            std::cout << "there is no snapshot" << std::endl;
            return;
        }
        if (!snapshot->topAsks.empty()) {  // TMP
            std::cout << "snapshot.topAsks[0].id = " << snapshot->topAsks[0].id << std::endl;
        }
        auto cmd = std::make_unique<BroadcastSnapshotCommand>(std::move(*snapshot));
        broadcaster_->enqueue(std::move(cmd));
    } catch (const std::exception& e) {
        std::cerr << "Failed to send snapshot: " << e.what() << std::endl;
        // stop_broadcasting();
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
    if (broadcasting_)
        return;
    start_broadcasting(cmd.interval_ms, cmd.use_test_broadcast);
}

void DOMManager::send_mdupdate(bool use_test_broadcast) {
    try {
        std::optional<common::MDUpdate> md_update = order_book_->GenerateMDUpdate(); // Предполагаемый метод
        if (!md_update) {
            std::cout << "there is no md update" << std::endl;
            return;
        }
        auto cmd = std::make_unique<BroadcastMDUpdateCommand>(std::move(*md_update));
        broadcaster_->enqueue(std::move(cmd));
    } catch (const std::exception& e) {
        std::cerr << "Failed to send md update: " << e.what() << std::endl;
    }
}

void DOMManager::send_mdupdate_by_client_id(ClientId client_id, bool use_test_broadcast) {
    try {
        std::optional<common::MDUpdate> md_update = order_book_->GenerateMDUpdate();
        if (!md_update) {
            std::cout << "there is no md update for client " << client_id << std::endl;
            return;
        }
        auto cmd = std::make_unique<BroadcastMDUpdateCommand>(client_id, std::move(*md_update));
        broadcaster_->enqueue(std::move(cmd));
    } catch (const std::exception& e) {
        std::cerr << "Failed to send md update to client " << client_id << ": " << e.what() << std::endl;
    }
}

void DOMManager::Handle(const SendMDUpdateToClientCmd& cmd) {
    send_mdupdate_by_client_id(cmd.client_id, cmd.use_test_broadcast);
}
   
}