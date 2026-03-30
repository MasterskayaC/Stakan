#include <iostream>
#include "app.h"
#include "menu.h"
#include "menu_action.h"

namespace app {

/// @brief Конструктор
Application::Application() {
    menu_.reset(new menu::Menu(std::cin, std::cout, snapshot_client_));
    command_handlers_.reset(new menu::CommandHandlers(*menu_, std::cout, snapshot_client_));
    args_parsers_.reset(new menu::ArgsParsers(*menu_, std::cin));
}

/// @brief Деструктор
Application::~Application() {
    stop_updates();
}

/// @brief Запуск приложения
void Application::Run() {
    std::cout << "Snapshot Console Client\n";
    menu_->PrintHelp();
    menu_->Run();
}

/// @brief Подключение к серверу
bool Application::connect(const std::string& host, uint16_t port) {
    return true;
}

/// @brief Запрос снапшота
bool Application::fetch_snapshot() {
    return true;
}

/// @brief Подписка на обновления
void Application::start_updates() {
}

/// @brief Отписка от обновлений
void Application::stop_updates() {
}

/// @brief Обработка полученного снапшота
void Application::on_snapshot_received(const common::Snapshot& snapshot) {
}

/// @brief Обработка полученного обновления
void Application::on_update_received(const tcp_client::TopOfBook& update) {
}

/// @brief Обработка ошибки
void Application::on_error(const std::string& error) {
}

} // namespace app
