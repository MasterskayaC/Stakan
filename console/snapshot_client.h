#pragma once

#include <cstdint>
#include <string>

namespace console {

/**
 * @brief Console client for snapshot retrieval and updates.
 */
class SnapshotConsoleClient {
public:
    /**
     * @brief Constructor.
     */
    SnapshotConsoleClient();

    /**
     * @brief Connect to the order book server.
     * @param host Server host.
     * @param port Server port.
     */
    void connect_to_server(const std::string& host, uint16_t port);

    /**
     * @brief Fetch snapshot from the server.
     */
    void fetch_snapshot();

    /**
     * @brief Display real-time updates.
     */
    void display_realtime_updates();

private:
    std::string host_;
    uint16_t port_;
    bool connected_;
};

} // namespace console