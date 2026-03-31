#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <array>

namespace console {

struct Order {
    uint64_t id = 0;
    uint64_t price = 0;
    int quantity = 0;
};

struct Snapshot {
    std::array<Order, 20> topBids;
    std::array<Order, 20> topAsks;
};

/// @brief Колбэк при получении снапшота
using SnapshotCallback = std::function<void(const Snapshot&)>;
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
    /// @brief Проверка соединения
    bool is_connected() const;

    /// @brief Установка колбэка снапшота
    void set_snapshot_callback(SnapshotCallback callback);
    /// @brief Установка колбэка ошибок
    void set_error_callback(ErrorCallback callback);

private:
    /// @brief Обработка подключения
    void on_connected();
    /// @brief Обработка отключения
    void on_disconnected();
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
    ErrorCallback error_callback_;
};

} // namespace console
