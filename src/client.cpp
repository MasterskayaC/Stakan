#include "tcp_client/client.hpp"

#include <utility>

namespace tcp_client {

struct TcpClient::Impl {
    ClientConfig config{};
    IClientCallbacks* callbacks{nullptr};
    bool connected{false};
};

TcpClient::TcpClient(ClientConfig config, IClientCallbacks* callbacks)
    : impl_(std::make_unique<Impl>(Impl{std::move(config), callbacks, false})) {}

TcpClient::~TcpClient() = default;

bool TcpClient::Connect() {
    // Здесь будет подключение к TCP-серверу и запуск фонового цикла чтения.
    return false;
}

void TcpClient::Disconnect() {
    // Здесь будет корректное закрытие сокета и остановка рабочих потоков.
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

}  // namespace tcp_client
