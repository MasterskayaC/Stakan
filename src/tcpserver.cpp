#include "../include/tcpserver.h"

TCPServer::TCPServer(boost::asio::io_context& io_context, unsigned short port)
    : io_context_(io_context)
    , acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
}

void TCPServer::StartServer() {
    do_accept();
}

void TCPServer::StopServer() {
    acceptor_.close();

    //need session:: Stop() for all session
}

void TCPServer::do_accept() {
    auto socket = std::make_shared<tcp::socket>(io_context_);

    acceptor_.async_accept(*socket,
                           boost::bind(&TCPServer::on_accept, this, socket,
                                       boost::asio::placeholders::error));
}

std::shared_ptr<Session> TCPServer::on_accept(std::shared_ptr<tcp::socket> socket,
                          const boost::system::error_code& error) {
    std::shared_ptr<Session> session;

    //create session

    do_accept();
    return session;
}

void TCPServer::update_message(const std::string& message) {

}
