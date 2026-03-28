#include "../include/tcp_client/client.hpp"
#include <boost/asio.hpp>
#include <utility>

namespace tcp_client {

struct TcpClient::Impl {
    explicit Impl(ClientConfig cfg, IClientCallbacks* cb)
        : config(std::move(cfg)), callbacks(cb), socket(io_context) {}

    ClientConfig config{};
    IClientCallbacks* callbacks{nullptr};
    bool connected{false};
    boost::asio::io_context io_context{};
    boost::asio::ip::tcp::socket socket;
};

TcpClient::TcpClient(ClientConfig config, IClientCallbacks* callbacks)
    : impl_(std::make_unique<Impl>(std::move(config), callbacks)) {}

TcpClient::~TcpClient() = default;

bool TcpClient::Connect() {
    // Здесь будет подключение через Boost.Asio и запуск цикла обработки
    return false;
}

void TcpClient::Disconnect() {
    // Здесь будет корректное закрытие сокета Boost.Asio и остановка
}

bool TcpClient::Subscribe(std::string symbol) {
    // Здесь будет отправка команды подписки на указанный инструмент.
    (void)symbol;
    return false;
}

bool TcpClient::Unsubscribe(std::string symbol) {
    // Здесь будет отправка команды отписки от указанного инструмента.
    (void)symbol;
    return false;
}

bool TcpClient::IsConnected() const {
    return impl_->connected;
}

}  
