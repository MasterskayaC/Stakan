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
    //заглушка
}

void SnapshotConsoleClient::fetch_snapshot() {
    //заглушка
}

void SnapshotConsoleClient::display_realtime_updates() {
    //заглушка
}

} // namespace console