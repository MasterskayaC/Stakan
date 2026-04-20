#pragma once

#include "types.hpp"
#include "../../bid_ask_snaphot_interface/bid_ask_interface.h"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#pragma once

namespace tcp_client {

    // Параметры подключения клиента.
    struct ClientConfig {
        std::string host{"127.0.0.1"};
        std::uint16_t port{9000};
        std::string client_name{"tcp-client"};
    };

    // интерфейс колбэков для приложения или UI.
    class IClientCallbacks {
    public:
        virtual ~IClientCallbacks() = default;

        // Вызывается после успешного подключения.
        virtual void OnConnected() = 0;
        // Вызывается при закрытии или потере соединения.
        virtual void OnDisconnected() = 0;
        // Вызывается при обновлении снапшота топ-бид/аск.
        virtual void OnTopOfBook(const common::Snapshot& snapshot) = 0;
        // Вызывается при ошибках транспорта/протокола.
        virtual void OnError(std::string_view message) = 0;
    };

    // Публичный API TCP-клиента.
    class TcpClient {
    public:
        explicit TcpClient(ClientConfig config, IClientCallbacks* callbacks);
        ~TcpClient();

        TcpClient(const TcpClient&) = delete;
        TcpClient& operator=(const TcpClient&) = delete;
        TcpClient(TcpClient&&) = delete;
        TcpClient& operator=(TcpClient&&) = delete;

        // Открывает TCP-соединение.
        bool Connect() const;
        // Закрывает соединение и останавливает фоновую работу.
        void Disconnect() const;

        // Подписка на обновления по инструменту.
        bool Subscribe(std::string symbol) const;
        // Отписка от обновлений по инструменту.
        bool Unsubscribe(std::string symbol) const;

        // Возвращает состояние подключения.
        bool IsConnected() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}  // namespace tcp_client
