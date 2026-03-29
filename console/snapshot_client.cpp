#include "snapshot_client.h"

#include <iostream>

namespace console {

SnapshotConsoleClient::SnapshotConsoleClient()
    : host_("127.0.0.1")
    , port_(0)
    , connected_(false) {}

void SnapshotConsoleClient::connect_to_server(const std::string& host, uint16_t port) {
    host_ = host;
    port_ = port;
    connected_ = true;
}

void SnapshotConsoleClient::fetch_snapshot() {
    if (!connected_) {
        std::cerr << "Not connected to server\n";
        return;
    }
}

void SnapshotConsoleClient::display_realtime_updates() {
    if (!connected_) {
        std::cerr << "Not connected to server\n";
        return;
    }
}

} 