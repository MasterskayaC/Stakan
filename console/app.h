#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "snapshot_client.h"

namespace menu {
class Menu;
class CommandHandlers;
class ArgsParsers;
} // namespace menu

namespace app {

/// @brief Главный класс приложения
class Application final {
public:
    /// @brief Конструктор
    explicit Application();
    /// @brief Деструктор
    ~Application();

    /// @brief Запуск приложения
    void Run();
    /// @brief Подключение к серверу
    bool connect(const std::string& host, uint16_t port);
    /// @brief Запрос снапшота
    bool fetch_snapshot();
    /// @brief Подписка на обновления
    void start_updates();
    /// @brief Отписка от обновлений
    void stop_updates();

private:
    /// @brief Обработка полученного снапшота
    void on_snapshot_received(const common::Snapshot& snapshot);
    /// @brief Обработка полученного обновления
    void on_update_received(const tcp_client::TopOfBook& update);
    /// @brief Обработка ошибки
    void on_error(const std::string& error);

private:
    console::SnapshotConsoleClient snapshot_client_;
    std::unique_ptr<menu::Menu> menu_;
    std::unique_ptr<menu::CommandHandlers> command_handlers_;
    std::unique_ptr<menu::ArgsParsers> args_parsers_;
};

} // namespace app
