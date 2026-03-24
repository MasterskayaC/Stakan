#include "../include/tcpserver.h"

TCPServer::TCPServer(boost::asio::io_context& io_context, unsigned short port)
    : io_context_(io_context)
    , acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
}

TCPServer::~TCPServer() {

}

void TCPServer::StartServer() {
}

void TCPServer::StopServer() {

}

void TCPServer::DoAccept() {

}

std::shared_ptr<Session> TCPServer::OnAccept(std::shared_ptr<tcp::socket> socket,
                          const boost::system::error_code& error) {
    return {};
}

void TCPServer::SendUpdateMessage(const std::string& message) {

}
