#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <array>

#include "client_lib_interface.hpp"

namespace console {

struct Order {
    uint64_t id = 0;
    uint64_t price = 0;
    int quantity = 0;
};

struct Snapshot {
    std::array<Order, 20> bids;
    std::array<Order, 20> asks;
};

using SnapshotCallback = std::function<void(const Snapshot&)>;
using ErrorCallback = std::function<void(const std::string&)>;

class SnapshotConsoleClient {
public:
    explicit SnapshotConsoleClient(std::shared_ptr<client_lib::IOrderBookClient> client);
    ~SnapshotConsoleClient();

    void set_snapshot_callback(SnapshotCallback callback);
    void set_error_callback(ErrorCallback callback);

    void connect_to_server(const std::string& host, uint16_t port);
    void disconnect_from_server();
    void fetch_snapshot();
    bool is_connected() const;

private:
    void on_connected();
    void on_disconnected();
    void on_snapshot(const client_lib::Snapshot& snapshot);
    void on_error(client_lib::ClientError error, std::string_view message);

private:
    std::shared_ptr<client_lib::IOrderBookClient> client_;
    client_lib::ClientConfig config_;
    SnapshotCallback snapshot_callback_;
    ErrorCallback error_callback_;
    bool connected_ = false;
};

} // namespace console
