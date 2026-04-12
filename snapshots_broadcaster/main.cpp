#include <iostream>
#include "dom_manager.h"

int main() {
    std::cout << "Eto birjevoy stakan!" << std::endl;

    // Create io_context and DOMManager
    boost::asio::io_context io_context;
    server::DOMManager dom(io_context);

    // Run io_context in a separate thread
    std::thread io_thread([&io_context]() {
        io_context.run();
        });

    // Send command to start broadcasting
    server::StartBroadcastCmd br{ 3000, true };
    dom(br);   // operator() passes the command to io_context

    // Simulate application work (10 seconds)
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // Stop broadcasting
    server::StopBroadcastCmd stop;
    dom(stop);

    // Stop io_context and wait for thread to finish
    io_context.stop();
    if (io_thread.joinable()) io_thread.join();

    std::cout << "Eto birjevoy stakan end!" << std::endl;
    return 0;
}