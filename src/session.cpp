#include "../include/session.h"

using namespace boost::asio::ip;

Session::Session(std::shared_ptr<tcp::socket> socket)
    : socket_(std::move(socket)) {}

void Session::Start() {
    Read();
}

void Session::Stop() {
    boost::system::error_code ec;
    socket_->close(ec);
}

void Session::Read() {
    socket_->async_read_some(
        boost::asio::buffer(read_buffer_),
        boost::bind(&Session::ProcessRead, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Session::ProcessRead(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        std::string message(read_buffer_.data(), bytes_transferred);
        Read();
    } else {
        std::cout << "Client disconnected: " << error.message() << std::endl;
        Stop();
    }
}

void Session::SendMsg(const std::string& message) {
    std::lock_guard<std::mutex> lock(write_mutex_);
    messages_queue_.push(message);

    if (!is_writing_) {
        Write();
    }
}

void Session::Write() {
    if (messages_queue_.empty()) {
        is_writing_ = false;
        return;
    }

    is_writing_ = true;
    const std::string& message = messages_queue_.front();
    boost::asio::async_write(
        *socket_,
        boost::asio::buffer(message),
        boost::bind(&Session::ProcessWrite, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Session::ProcessWrite(const boost::system::error_code& error, size_t bytes_transferred) {
    std::lock_guard<std::mutex> lock(write_mutex_);   

    if (!error) {
        messages_queue_.pop();
        Write();
    } else {
        std::cout << "Write error: " << error.message() << std::endl;
        Stop();
    }
}

