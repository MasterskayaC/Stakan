#include "snapshot_client.h"

#include <utility>

namespace console {

SnapshotConsoleClient::SnapshotConsoleClient(std::shared_ptr<client_lib::IOrderBookClient> client)
    : client_(std::move(client))
    , connected_(false) {
}

SnapshotConsoleClient::~SnapshotConsoleClient() {
    if (connected_) {
        disconnect_from_server();
    }
}

void SnapshotConsoleClient::set_snapshot_callback(SnapshotCallback callback) {
    snapshot_callback_ = std::move(callback);
}

void SnapshotConsoleClient::set_error_callback(ErrorCallback callback) {
    error_callback_ = std::move(callback);
}

void SnapshotConsoleClient::connect_to_server(const std::string& host, uint16_t port) {
    config_.host = host;
    config_.port = port;

    client_lib::ClientCallbacks callbacks;
    callbacks.on_connected = [this]() { on_connected(); };
    callbacks.on_disconnected = [this]() { on_disconnected(); };
    callbacks.on_snapshot = [this](const client_lib::Snapshot& snap) { on_snapshot(snap); };
    callbacks.on_error = [this](client_lib::ClientError err, std::string_view msg) { on_error(err, msg); };

    client_->SetCallbacks(std::move(callbacks));
    client_->Connect(config_);
    client_->Start();
}

void SnapshotConsoleClient::disconnect_from_server() {
    client_->Disconnect();
    client_->Stop();
    connected_ = false;
}

void SnapshotConsoleClient::fetch_snapshot() {
    if (!connected_) {
        return;
    }
    client_->RequestSnapshot();
}

bool SnapshotConsoleClient::is_connected() const {
    return client_->IsConnected();
}

void SnapshotConsoleClient::on_connected() {
    connected_ = true;
}

void SnapshotConsoleClient::on_disconnected() {
    connected_ = false;
}

void SnapshotConsoleClient::on_snapshot(const client_lib::Snapshot& snapshot) {
    if (snapshot_callback_) {
        snapshot_callback_(snapshot);
    }
}

void SnapshotConsoleClient::on_error(client_lib::ClientError error, std::string_view message) {
    if (error_callback_) {
        error_callback_(std::string(message));
    }
}

} // namespace console
