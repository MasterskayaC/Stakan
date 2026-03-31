#pragma once

#include <functional>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

#include "snapshot_client.h"

namespace menu {

/// @brief Без аргументов
using NoArgs = std::monostate;
/// @brief Аргументы команды
using CommandArgs = std::variant<NoArgs>;
/// @brief Обработчик команды
using CommandHandler = std::function<bool(CommandArgs&)>;
/// @brief Парсер аргументов
using ArgParser = std::function<CommandArgs(std::istream&)>;

/// @brief Меню команд
class Menu final {
public:
    /// @brief Конструктор
    Menu(std::istream& input, std::ostream& output, std::shared_ptr<console::SnapshotConsoleClient> client);

    /// @brief Зарегистрировать обработчик команды
    void RegisterHandler(const std::string& command, const std::string& description, CommandHandler handler);

    /// @brief Запуск обработки команд
    void Run();
    /// @brief Остановка обработки команд
    void Stop();
    /// @brief Печать справки
    void PrintHelp();

private:
    /// @brief Описание команды
    struct Command {
        std::string command;
        std::string description;
        CommandHandler handler;
    };

    using CommandPtr = std::shared_ptr<Command>;

    /// @brief Парсинг и выполнение команды
    [[nodiscard]] bool ParseCommand(std::istream& input);

private:
    using CommandHashTable = std::unordered_map<std::string_view, CommandPtr>;

    std::istream& input_;
    std::ostream& output_;
    std::shared_ptr<console::SnapshotConsoleClient> client_;

    CommandHashTable commands_;
    bool running_ = false;
};

} // namespace menu
