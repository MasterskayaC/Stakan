#include "../include/session.h"

using namespace boost::asio::ip;

Session::Session(std::shared_ptr<tcp::socket> socket)
    : socket_(std::move(socket)) {}

void Session::Start() {

}

void Session::Stop() {

}

void Session::Read() {

}

void Session::ProcessRead(const boost::system::error_code& error, size_t bytes_transferred) {

}

void Session::SendMsg(const std::string& message) {

}

void Session::Write() {    

}

void Session::ProcessWrite(const boost::system::error_code& error, size_t bytes_transferred) {

}

