#pragma once

#include "order_book_client.hpp"
#include "snapshot_client.h"

namespace app {

class Application final {
public:
    /**
     * @brief The constructor establishes a connection to the server
     */
    explicit Application();

    /**
     * @brief The method starts the application
     */
    void Run();

private:
    client_lib::OrderBookClient client_;
    console::SnapshotConsoleClient snapshot_client_;
};

} 
