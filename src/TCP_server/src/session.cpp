#include "../include/session.h"

#include <istream>
#include <utility>

using namespace boost::asio::ip;

// Реализация асинхронного цикла: read_until('\n') для команд и async_write для ответов/снэпшотов.
Session::Session(std::shared_ptr<tcp::socket> socket)
    : socket_(std::move(socket)) {}

void Session::Start() {
    Read();
}

void Session::Stop() {
    boost::system::error_code ec;
    if (socket_ && socket_->is_open()) {
        socket_->shutdown(tcp::socket::shutdown_both, ec);
        socket_->close(ec);
    }
}

void Session::Read() {
    auto self = shared_from_this();
    boost::asio::async_read_until(
        *socket_,
        read_buffer_,
        '\n',
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            ProcessRead(error, bytes_transferred);
        });
}

void Session::ProcessRead(const boost::system::error_code& error, std::size_t bytes_transferred) {
    (void)bytes_transferred;
    if (error) {
        if (on_disconnect_) {
            on_disconnect_(shared_from_this());
        }
        return;
    }

    std::istream is(&read_buffer_);
    std::string line;
    std::getline(is, line);
    if (on_line_) {
        on_line_(line, shared_from_this());
    }
    Read();
}

void Session::SendMsg(const std::vector<char>& message) {
    if (!socket_ || !socket_->is_open()) {
        return;
    }

    bool start_writing = false;
    {
        std::lock_guard<std::mutex> lock(write_mutex_);
        start_writing = messages_queue_.empty();
        messages_queue_.push_back(message);
    }

    if (start_writing) {
        Write();
    }
}

bool Session::IsOpen() const {
    return socket_ && socket_->is_open();
}

void Session::SetCallbacks(OnLineCallback on_line, OnDisconnectCallback on_disconnect) {
    on_line_ = std::move(on_line);
    on_disconnect_ = std::move(on_disconnect);
}

void Session::Write() {
    std::vector<char> data;
    {
        std::lock_guard<std::mutex> lock(write_mutex_);
        if (messages_queue_.empty()) {
            return;
        }
        data = messages_queue_.front();
    }

    auto self = shared_from_this();
    boost::asio::async_write(
        *socket_,
        boost::asio::buffer(data),
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            ProcessWrite(error, bytes_transferred);
        });
}

void Session::ProcessWrite(const boost::system::error_code& error, std::size_t bytes_transferred) {
    (void)bytes_transferred;
    if (error) {
        if (on_disconnect_) {
            on_disconnect_(shared_from_this());
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lock(write_mutex_);
        if (!messages_queue_.empty()) {
            messages_queue_.pop_front();
        }
    }
    Write();
}
