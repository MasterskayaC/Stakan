#include "../include/session.h"

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

    std::vector<char> data(bytes_transferred);
    boost::asio::buffer_copy(boost::asio::buffer(data), read_buffer_.data(), bytes_transferred);
    read_buffer_.consume(bytes_transferred);
    if (on_data_) {
        on_data_(data, shared_from_this());
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
        // Переход очереди из пустой в непустую защищен write_mutex_,
        // поэтому локальному флагу не нужна атомарность.
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

void Session::SetCallbacks(OnDataCallback on_data, OnDisconnectCallback on_disconnect) {
    on_data_ = std::move(on_data);
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
