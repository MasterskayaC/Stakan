#include "../include/session.h"

#include <memory>
#include <utility>

using namespace boost::asio::ip;

// Чтение: async_read_until по '\n' (разделитель кадра), данные в колбэк как std::vector<std::uint8_t>.
// Отправка: постановка в очередь под mutex, старт цепочки async_write через dispatch на executor сокета.
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
    if (error) {
        if (on_disconnect_) {
            on_disconnect_(shared_from_this());
        }
        return;
    }

    std::vector<std::uint8_t> frame(bytes_transferred);
    if (bytes_transferred > 0) {
        boost::asio::buffer_copy(boost::asio::buffer(frame), read_buffer_.data(), bytes_transferred);
    }
    read_buffer_.consume(bytes_transferred);

    if (on_data_) {
        on_data_(frame, shared_from_this());
    }
    Read();
}

void Session::SendMsg(const std::vector<char>& message) {
    if (!socket_) {
        return;
    }

    auto self = shared_from_this();
    auto payload = std::make_shared<std::vector<char>>(message);
    boost::asio::dispatch(socket_->get_executor(), [this, self, payload]() {
        if (!socket_ || !socket_->is_open()) {
            return;
        }
        bool start_chain = false;
        {
            std::lock_guard<std::mutex> lock(write_mutex_);
            start_chain = messages_queue_.empty();
            messages_queue_.push_back(std::move(*payload));
        }
        if (start_chain) {
            Write();
        }
    });
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
