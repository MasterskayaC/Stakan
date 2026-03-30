#pragma once

#include <deque>
#include <functional>
#include <iosfwd>
#include <memory>
#include <set>
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
    Menu(std::istream& input, std::ostream& output, console::SnapshotConsoleClient& client);

    /// @brief Добавить команду
    void AddCommand(const std::string& command, const std::string& description, CommandHandler handler);
    /// @brief Добавить парсер аргументов
    void AddArgsParser(const std::string& command, std::set<std::string>&& args, ArgParser parser);
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
        std::set<std::string> args;
        std::string description;
        ArgParser parser;
        CommandHandler handler;
    };

    using CommandPtr = std::shared_ptr<Command>;

    /// @brief Парсинг команды
    [[nodiscard]] bool ParseCommand(std::istream& input);

private:
    using CommandDeque = std::deque<CommandPtr>;
    using CommandHashTable = std::unordered_map<std::string_view, CommandPtr>;

    std::istream& input_;
    std::ostream& output_;
    console::SnapshotConsoleClient& client_;

    CommandDeque commands_;
    CommandHashTable name_to_commands_;
    bool running_ = false;
};

} // namespace menu
