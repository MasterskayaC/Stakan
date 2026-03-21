#include "session.h"

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <mutex>

using boost::asio::ip::tcp;

class TCPServer {
public:
    TCPServer(boost::asio::io_context& io_context, unsigned short port);
    ~TCPServer();

    void StartServer(); // start server
    void StopServer();  // stop server

    void update_message(const std::string& message); // send update msg to all client

private:
    void do_accept(); // init async waiting new connection
    std::shared_ptr<Session> on_accept(std::shared_ptr<tcp::socket> socket,
                   const boost::system::error_code& error); // get session to sessions container

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;

    //std::vector<std::shared_ptr<class Session>> sessions_; // change to unordered_map and do id is key ??
    mutable std::mutex sessions_mutex_;
};
