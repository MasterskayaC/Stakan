#include "menu.h"

#include <algorithm>
#include <iostream>

#include "menu_action.h"

namespace menu {

/// @brief Конструктор
Menu::Menu(std::istream& input, std::ostream& output, console::SnapshotConsoleClient& client)
    : input_(input)
    , output_(output)
    , client_(client) {
}

/// @brief Добавить команду
void Menu::AddCommand(const std::string& command,
                      const std::string& description,
                      menu::CommandHandler handler) {
}

/// @brief Добавить парсер аргументов
void Menu::AddArgsParser(const std::string& command,
                         std::set<std::string>&& args,
                         ArgParser parser) {
}

/// @brief Запуск обработки команд
void Menu::Run() {
    running_ = true;
    while (running_) {
        output_ << "> ";
        std::string cmd;
        if (!(input_ >> cmd)) {
            break;
        }

        if (cmd == "help") {
            PrintHelp();
        } else {
            ParseCommand(input_);
        }
    }
}

/// @brief Остановка обработки команд
void Menu::Stop() {
    running_ = false;
}

/// @brief Парсинг команды
bool Menu::ParseCommand(std::istream& input) {
    return false;
}

/// @brief Печать справки
void Menu::PrintHelp() {
    output_ << "Available commands:\n";
    for (const auto& cmd : commands_) {
        std::string args_str;
        for (const auto& arg : cmd->args) {
            args_str += "<" + arg + "> ";
        }
        output_ << "  " << cmd->command;
        if (!args_str.empty()) {
            output_ << " " << args_str;
        }
        output_ << " - " << cmd->description << "\n";
    }
}

} // namespace menu
