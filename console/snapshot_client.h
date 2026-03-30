#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

#include "client_lib/client_lib_interface.hpp"
#include "bid_ask_snaphot_interface/bid_ask_interface.h"
#include "tcp_client/types.hpp"

namespace console {

/// @brief Колбэк при получении снапшота
using SnapshotCallback = std::function<void(const common::Snapshot&)>;
/// @brief Колбэк при обновлении стакана
using UpdateCallback = std::function<void(const tcp_client::TopOfBook&)>;
/// @brief Колбэк при ошибке
using ErrorCallback = std::function<void(const std::string&)>;

/// @brief Консольный клиент для получения снапшотов и обновлений
class SnapshotConsoleClient {
public:
    /// @brief Конструктор
    SnapshotConsoleClient();
    /// @brief Деструктор
    ~SnapshotConsoleClient();

    /// @brief Подключение к серверу
    void connect_to_server(const std::string& host, uint16_t port);
    /// @brief Запрос снапшота
    void fetch_snapshot();
    /// @brief Подписка на обновления
    void display_realtime_updates();
    /// @brief Отписка от обновлений
    void stop_realtime_updates();
    /// @brief Проверка соединения
    bool is_connected() const;

    /// @brief Установка колбэка снапшота
    void set_snapshot_callback(SnapshotCallback callback);
    /// @brief Установка колбэка обновлений
    void set_update_callback(UpdateCallback callback);
    /// @brief Установка колбэка ошибок
    void set_error_callback(ErrorCallback callback);

private:
    /// @brief Обработка подключения
    void on_connected();
    /// @brief Обработка отключения
    void on_disconnected();
    /// @brief Обработка полученного снапшота
    void on_snapshot_received(const common::Snapshot& snapshot);
    /// @brief Обработка полученного обновления
    void on_update_received(const tcp_client::TopOfBook& update);
    /// @brief Обработка ошибки
    void on_error(const std::string& message);

private:
    /// @brief PIMPL
    struct Impl;
    std::unique_ptr<Impl> impl_;

    std::string host_;
    uint16_t port_;
    bool connected_;

    SnapshotCallback snapshot_callback_;
    UpdateCallback update_callback_;
    ErrorCallback error_callback_;
};

} // namespace console
