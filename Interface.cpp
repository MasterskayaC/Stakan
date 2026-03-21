#pragma once

#include <cstdint>
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace client_lib {

    // Определены у сервера
    struct TopLevel;
    struct Snapshot;

    enum class ConnectionState {
        Stopped,
        Connecting,
        Connected,
        Reconnecting,
        Disconnected
    };

    enum class ClientError {
        None,
        SocketOpenFailed,
        ConnectFailed,
        ReadFailed,
        WriteFailed,
        ProtocolError,
        UnexpectedMessage,
        SnapshotRequestFailed
    };

    struct ClientConfig {
        std::string host{ "127.0.0.1" };
        std::uint16_t port{ 0 };
        bool auto_reconnect{ true };
        std::chrono::milliseconds reconnect_delay{ 1000 };
    };

    struct ClientCallbacks {
        std::function<void()> on_connected;
        std::function<void()> on_disconnected;
        std::function<void(ConnectionState)> on_state_changed;
        std::function<void(const Snapshot&)> on_snapshot;
        std::function<void(const TopLevel&)> on_top_of_book;
        std::function<void(ClientError, std::string_view)> on_error;
    };

    class IOrderBookClient {
    public:
        virtual ~IOrderBookClient() = default;

        virtual void SetCallbacks(ClientCallbacks callbacks) = 0;

        virtual void Connect(const ClientConfig& config) = 0;
        virtual void Disconnect() = 0;

        virtual void Start() = 0;
        virtual void Stop() = 0;

        virtual void RequestSmapshot() = 0;

        virtual bool IsConnected() const noexcept = 0;
        virtual ConnectionState State() const noexcept = 0;
    };

} // namespace client_lib