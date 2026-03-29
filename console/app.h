#pragma once

#include <cstdint>
#include <string>
#include "snapshot_client.h"

namespace app {

/// @brief Главный класс приложения для консольного клиента
///
/// Координирует пользовательский ввод, подключение к серверу и
/// отображение снапшотов в интерактивном командном цикле
class Application final {
public:
    /// @brief Конструируем приложение и инициализируем клиент снапшотов
    explicit Application();

    /// @brief Деструктор, гарантирует остановку обновлений при выходе
    ~Application();

    /// @brief Запускаем интерактивный командный цикл
    void Run();

    /// @brief Печатаем доступные команды в stdout
    void print_help();

    /// @brief Устанавливаем подключение к серверу стакана заказов
    bool connect(const std::string& host, uint16_t port);

    /// @brief Запрашиваем снапшот у сервера
    /// @return true, если запрос успешно отправлен
    bool fetch_snapshot();

    /// @brief Начинаем приём обновлений стакана заказов в реальном времени
    void start_updates();

    /// @brief Останавливаем приём обновлений стакана заказов в реальном времени
    void stop_updates();

private:
    console::SnapshotConsoleClient snapshot_client_;
    bool running_ = false;
};

}  
