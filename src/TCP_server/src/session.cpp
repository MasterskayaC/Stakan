#include "../include/session.h"

#include <memory>
#include <utility>

using namespace boost::asio::ip;

// Чтение/запись сериализуются через strand_ (см. docs boost::asio::strand),
// поэтому нам не нужен mutex для очереди отправки.
Session::Session(std::shared_ptr<tcp::socket> socket)
    : socket_(std::move(socket))
    , strand_(boost::asio::make_strand(socket_->get_executor())) {}

void Session::Start() {
    auto weak = weak_from_this();
    boost::asio::dispatch(strand_, [weak]() {
        if (auto self = weak.lock()) {
            self->Read();
        }
    });
}

void Session::Stop() {
    auto weak = weak_from_this();
    boost::asio::dispatch(strand_, [weak]() {
        auto self = weak.lock();
        if (!self || self->disconnected_) {
            return;
        }
        self->disconnected_ = true;
        boost::system::error_code ec;
        if (self->socket_ && self->socket_->is_open()) {
            self->socket_->shutdown(tcp::socket::shutdown_both, ec);
            self->socket_->close(ec);
        }
    });
}

void Session::Read() {
    if (!socket_ || !socket_->is_open()) {
        NotifyDisconnectOnce();
        return;
    }

    auto weak = weak_from_this();
    boost::asio::async_read_until(
        *socket_,
        read_buffer_,
        '\n',
        boost::asio::bind_executor(
            strand_,
            [weak](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (auto self = weak.lock()) {
                    self->ProcessRead(error, bytes_transferred);
                }
            }));
}

void Session::ProcessRead(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error) {
        // TODO: выделить отдельную обработку EOF/reset/operation_aborted.
        NotifyDisconnectOnce();
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

void Session::SendMsg(std::vector<char> message) {
    auto payload = std::make_shared<std::vector<char>>(std::move(message));
    auto weak = weak_from_this();
    boost::asio::dispatch(strand_, [weak, payload]() {
        auto self = weak.lock();
        if (!self || self->disconnected_ || !self->socket_ || !self->socket_->is_open()) {
            return;
        }
        self->messages_queue_.push_back(payload);
        if (!self->write_in_progress_) {
            self->write_in_progress_ = true;
            self->Write();
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
    if (!socket_ || !socket_->is_open()) {
        write_in_progress_ = false;
        NotifyDisconnectOnce();
        return;
    }
    if (messages_queue_.empty()) {
        write_in_progress_ = false;
        return;
    }

    auto chunk = messages_queue_.front();
    auto weak = weak_from_this();
    boost::asio::async_write(
        *socket_,
        boost::asio::buffer(*chunk),
        boost::asio::bind_executor(
            strand_,
            [weak, chunk](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (auto self = weak.lock()) {
                    self->ProcessWrite(error, bytes_transferred);
                }
            }));
}

void Session::ProcessWrite(const boost::system::error_code& error, std::size_t bytes_transferred) {
    (void)bytes_transferred;
    if (error) {
        // TODO: отдельно обрабатывать operation_aborted (локальный stop) и сетевые ошибки.
        write_in_progress_ = false;
        NotifyDisconnectOnce();
        return;
    }

    if (!messages_queue_.empty()) {
        messages_queue_.pop_front();
    }
    if (messages_queue_.empty()) {
        write_in_progress_ = false;
        return;
    }
    Write();
}

void Session::NotifyDisconnectOnce() {
    if (disconnected_) {
        return;
    }
    disconnected_ = true;
    write_in_progress_ = false;
    messages_queue_.clear();

    if (on_disconnect_) {
        on_disconnect_(shared_from_this());
    }
}
