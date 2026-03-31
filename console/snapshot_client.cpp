#include "snapshot_client.h"

#include <iostream>

namespace console {

/// @brief Конструктор
SnapshotConsoleClient::SnapshotConsoleClient()
    : host_("127.0.0.1")
    , port_(0)
    , connected_(false) {
    impl_ = nullptr;
}

/// @brief Деструктор
SnapshotConsoleClient::~SnapshotConsoleClient() {
    if (connected_) {
        on_disconnected();
    }
}

/// @brief Подключение к серверу
void SnapshotConsoleClient::connect_to_server(const std::string& host, uint16_t port) {
    host_ = host;
    port_ = port;
    connected_ = true;
}

/// @brief Запрос снапшота
void SnapshotConsoleClient::fetch_snapshot() {
    if (!connected_) {
        std::cerr << "Not connected to server\n";
        return;
    }
}

/// @brief Проверка соединения
bool SnapshotConsoleClient::is_connected() const {
    return connected_;
}

/// @brief Установка колбэка снапшота
void SnapshotConsoleClient::set_snapshot_callback(SnapshotCallback callback) {
    snapshot_callback_ = std::move(callback);
}

/// @brief Установка колбэка ошибок
void SnapshotConsoleClient::set_error_callback(ErrorCallback callback) {
    error_callback_ = std::move(callback);
}

/// @brief Обработка подключения
void SnapshotConsoleClient::on_connected() {
    connected_ = true;
}

/// @brief Обработка отключения
void SnapshotConsoleClient::on_disconnected() {
    connected_ = false;
}

/// @brief Обработка ошибки
void SnapshotConsoleClient::on_error(const std::string& message) {
    if (error_callback_) {
        error_callback_(message);
    }
}

} // namespace console
