#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

#include "client_lib/client_lib_interface.hpp"
#include "bid_ask_snaphot_interface/bid_ask_interface.h"
#include "tcp_client/types.hpp"

namespace console {

/// @brief вызываемый при получении снапшота от сервера
using SnapshotCallback = std::function<void(const common::Snapshot&)>;

/// @brief Квызываем при обновлении top-of-book.
using UpdateCallback = std::function<void(const tcp_client::TopOfBook&)>;

/// @brief Еще один колбэк, при ошибке
using ErrorCallback = std::function<void(const std::string&)>;

/// @brief Консолька для получения снапшотов и обновлений стакана заказов
///
/// Предоставляет функционал для подключения к серверу стакана,
/// запроса снапшотов и получения обновлений ИРЛ
class SnapshotConsoleClient {
public:
    /// @brief Конструктор, создающий новый консольный клиент с настройками по умолчанию
    SnapshotConsoleClient();

    /// @brief Деструктор
    ~SnapshotConsoleClient();

    /// @brief Подключаемся к серверу стакана заказов
    void connect_to_server(const std::string& host, uint16_t port);

    /// @brief Запрашивает снапшот текущего состояния стакана заказов
    void fetch_snapshot();

    /// @brief Начинает приём и отображение обновлений ИРЛ
    void display_realtime_updates();

    /// @brief Устанавливает колбэк для получения снапшотов
    void set_snapshot_callback(SnapshotCallback callback);

    /// @param[in] callback Функция, вызываемая при получении обновления
    void set_update_callback(UpdateCallback callback);

    /// @param[in] callback Ф-ия, вызываемая при возникновении ошибки
    void set_error_callback(ErrorCallback callback);

private:
    /// @brief Обрабатывает успешное подключение а затем отключение к серверу
    void on_connected();
    void on_disconnected();

    /// @brief Обрабатывает полученный снапшот
    void on_snapshot_received(const common::Snapshot& snapshot);

    /// @brief Обрабатывает полученное обновление
    void on_update_received(const tcp_client::TopOfBook& update);

    /// @brief Обрабатывает условия ошибки
    void on_error(const std::string& message);

private:
    /// @brief Паттерн PIMPL для сокрытия деталей реализации
    struct Impl;

    std::unique_ptr<Impl> impl_;

    std::string host_;
    uint16_t port_;
    bool connected_;

    SnapshotCallback snapshot_callback_;
    UpdateCallback update_callback_;
    ErrorCallback error_callback_;
};

}  
