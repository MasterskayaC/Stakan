#include "include/tcpserver.h"

#include <iostream>

int main(int argc, char *argv[])
{
    try {
        boost::asio::io_context io_context;
        short port = 1234;
        auto tcp_server = std::make_shared<TCPServer>(io_context, port);
        tcp_server->StartServer();

        std::cout << "Server started" << std::endl;

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }


    return 0;
}
