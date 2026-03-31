#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "snapshot_client.h"

namespace menu {
class Menu;
class CommandHandlers;
} // namespace menu

namespace app {

class Application final {
public:
    explicit Application();
    ~Application();

    void Run();
    bool connect(const std::string& host, uint16_t port);
    bool fetch_snapshot();

private:
    void on_snapshot_received(const console::Snapshot& snapshot);
    void on_error(const std::string& error);

private:
    std::shared_ptr<console::SnapshotConsoleClient> snapshot_client_;
    std::unique_ptr<menu::Menu> menu_;
    std::unique_ptr<menu::CommandHandlers> command_handlers_;
};

} // namespace app
