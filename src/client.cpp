#include "../../include/tcp_client/client.hpp"
#include "../../bid_ask_snaphot_interface/bid_ask_interface.h"

#include <boost/asio.hpp>
#include <cstring>
#include <format>
#include <stdexcept>
#include <thread>
#include <vector>

#include "bid_ask_book/logger.h"

namespace tcp_client {

using boost::asio::ip::tcp;

namespace {

constexpr std::uint8_t kSnapshotMessageType = 0;
constexpr std::uint8_t kMdUpdateMessageType = 1;

std::vector<char> ToCharBuffer(const std::vector<std::uint8_t>& data) {
    return {data.begin(), data.end()};
}

void DispatchPayload(const std::vector<std::uint8_t>& data, IClientCallbacks* callbacks) {
    if (data.empty()) {
        throw std::runtime_error("Received empty payload");
    }

    const auto message_type = data[common::kPtrForMsgType];
    const auto payload = ToCharBuffer(data);

    switch (message_type) {
        case kSnapshotMessageType: {
            const auto snapshot = common::Snapshot::deserialize(payload);
            if (callbacks) {
                callbacks->OnTopOfBook(snapshot);
            }
            return;
        }
        case kMdUpdateMessageType: {
            const auto update = common::MDUpdate::deserialize(payload);
            if (callbacks) {
                callbacks->OnMarketDataUpdate(update);
            }
            return;
        }
        default:
            throw std::runtime_error(std::format("Unknown payload type: {}", static_cast<unsigned>(message_type)));
    }
}

}  // namespace

struct TcpClient::Impl {
    explicit Impl(ClientConfig cfg, IClientCallbacks* cb)
        : config(std::move(cfg)), callbacks(cb), socket(io_context) {}

    ClientConfig config{};
    IClientCallbacks* callbacks{nullptr};
    bool connected{false};

    boost::asio::io_context io_context{};
    tcp::socket socket;
    std::thread io_thread;
};

TcpClient::TcpClient(ClientConfig config, IClientCallbacks* callbacks)
    : impl_(std::make_unique<Impl>(std::move(config), callbacks)) {}

TcpClient::~TcpClient() {
    Disconnect();
}

bool TcpClient::Connect() const {
    if (impl_->connected) return true;

    try {
        tcp::resolver resolver(impl_->io_context);
        auto endpoints = resolver.resolve(impl_->config.host, std::to_string(impl_->config.port));
        boost::asio::connect(impl_->socket, endpoints);

        impl_->connected = true;
        server::Logger::Log(server::LogLevel::Info,
                            std::format("TCP client '{}' connected to {}:{}",
                                        impl_->config.client_name,
                                        impl_->config.host,
                                        impl_->config.port));

        if (impl_->callbacks) impl_->callbacks->OnConnected();

        impl_->io_thread = std::thread([this]() {
            try {
                while (impl_->connected) {
                    std::vector<uint8_t> size_buf(sizeof(uint32_t));
                    boost::asio::read(impl_->socket, boost::asio::buffer(size_buf));

                    uint32_t payload_size;
                    std::memcpy(&payload_size, size_buf.data(), sizeof(payload_size));
                    payload_size = ntohl(payload_size);

                    std::vector<uint8_t> data(payload_size);
                    boost::asio::read(impl_->socket, boost::asio::buffer(data));

                    DispatchPayload(data, impl_->callbacks);
                }
            } catch (const std::exception& e) {
                server::Logger::Log(server::LogLevel::Error,
                                    std::format("Payload read error: {}", e.what()));
                impl_->connected = false;
                if (impl_->callbacks) impl_->callbacks->OnDisconnected();
            }
        });

        return true;

    } catch (const std::exception& e) {
        server::Logger::Log(server::LogLevel::Error,
                            std::format("Connect error: {}", e.what()));
        if (impl_->callbacks) impl_->callbacks->OnError(e.what());
        return false;
    }
}

void TcpClient::Disconnect() const {
    if (!impl_->connected) return;

    try {
        impl_->connected = false;

        boost::system::error_code ec;
        impl_->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            server::Logger::Log(server::LogLevel::Warning,
                                std::format("Socket shutdown warning: {}", ec.message()));
        }
        impl_->socket.close(ec);
        if (ec) {
            server::Logger::Log(server::LogLevel::Warning,
                                std::format("Socket close warning: {}", ec.message()));
        }

        if (impl_->io_thread.joinable()) {
            impl_->io_thread.join();
        }

        server::Logger::Log(server::LogLevel::Info,
                            std::format("TCP client '{}' disconnected", impl_->config.client_name));

        if (impl_->callbacks) {
            impl_->callbacks->OnDisconnected();
        }

    } catch (const std::exception& e) {
        server::Logger::Log(server::LogLevel::Error,
                            std::format("Disconnect error: {}", e.what()));
        if (impl_->callbacks) impl_->callbacks->OnError(e.what());
    }
}

bool TcpClient::Subscribe(std::string symbol) const {
    if (!impl_->connected) return false;
    try {
        std::string msg = "SUBSCRIBE " + symbol + "\n";
        boost::asio::write(impl_->socket, boost::asio::buffer(msg));
        server::Logger::Log(server::LogLevel::Info, "Subscribed to: " + symbol);
        return true;
    } catch (const std::exception& e) {
        server::Logger::Log(server::LogLevel::Error, std::string("Subscribe error: ") + e.what());
        if (impl_->callbacks) impl_->callbacks->OnError(e.what());
        return false;
    }
}

bool TcpClient::Unsubscribe(std::string symbol) const {
    if (!impl_->connected) return false;
    try {
        std::string msg = "UNSUBSCRIBE " + symbol + "\n";
        boost::asio::write(impl_->socket, boost::asio::buffer(msg));
        server::Logger::Log(server::LogLevel::Info, "Unsubscribed from: " + symbol);
        return true;
    } catch (const std::exception& e) {
        server::Logger::Log(server::LogLevel::Error, std::string("Unsubscribe error: ") + e.what());
        if (impl_->callbacks) impl_->callbacks->OnError(e.what());
        return false;
    }
}

bool TcpClient::IsConnected() const {
    return impl_->connected;
}

}  // namespace tcp_client
