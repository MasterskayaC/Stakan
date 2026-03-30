#pragma once

#include <iosfwd>
#include <string>

#include "snapshot_client.h"
#include "menu.h"

namespace menu {

/// @brief Обработчики команд
class CommandHandlers final {
public:
    /// @brief Конструктор
    CommandHandlers(Menu& menu, std::ostream& output, console::SnapshotConsoleClient& client);

    /// @brief Команда connect
    bool Connect(CommandArgs&& args);
    /// @brief Команда snapshot
    bool Snapshot(CommandArgs&& args);
    /// @brief Команда subscribe
    bool Subscribe(CommandArgs&& args);
    /// @brief Команда unsubscribe
    bool Unsubscribe(CommandArgs&& args);
    /// @brief Команда help
    bool Help(CommandArgs&& args);
    /// @brief Команда exit
    bool Exit(CommandArgs&& args);

private:
    Menu& menu_;
    std::ostream& output_;
    console::SnapshotConsoleClient& client_;
};

/// @brief Парсеры аргументов
class ArgsParsers final {
public:
    /// @brief Конструктор
    ArgsParsers(Menu& menu, std::istream& input);
    /// @brief Парсинг connect
    CommandArgs ParseConnectArgs(std::istream& input);

private:
    Menu& menu_;
    std::istream& input_;
};

} // namespace menu
