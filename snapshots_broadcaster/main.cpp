#include <iostream>

#include "dom_manager.h"

int main() {
    std::cout << "Eto birjevoy stakan!" << std::endl;

    // Create io_context and DOMManager
    boost::asio::io_context io_context;
    int num_of_snapshots = 5;
    bool is_random = false;
    std::unique_ptr<ISnapshotSource> snapshot_source = makeTmpSnapshotCreator(is_random, num_of_snapshots);
    std::unique_ptr<IClientList> client_list = makeClientList();
    server::DOMManager dom(io_context, *client_list, std::move(snapshot_source));

    // Run io_context in a separate thread
    std::thread io_thread([&io_context]() {
        io_context.run();
    });

    // Send command to start broadcasting
    int broadcast_interval_ms = 3000;
    bool use_test_broadcast = true;
    server::StartBroadcastCmd br{broadcast_interval_ms, use_test_broadcast};
    dom(br);  // operator() passes the command to io_context

    // Simulate application work (10 seconds)
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // Stop broadcasting
    server::StopBroadcastCmd stop;
    dom(stop);

    // Stop io_context and wait for thread to finish
    io_context.stop();
    if (io_thread.joinable())
        io_thread.join();

    std::cout << "Eto birjevoy stakan end!" << std::endl;
    return 0;
}
