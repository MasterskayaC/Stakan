#include "menu_action.h"

#include <string>

#include "menu.h"

namespace menu {

/// @brief Конструктор
CommandHandlers::CommandHandlers(Menu& menu,
                                 std::ostream& output,
                                 std::shared_ptr<console::SnapshotConsoleClient> client)
    : menu_(menu)
    , output_(output)
    , client_(std::move(client)) {
}

/// @brief Команда connect
bool CommandHandlers::Connect(CommandArgs&& args) {
    return false;
}

/// @brief Команда snapshot
bool CommandHandlers::Snapshot(CommandArgs&& args) {
    if (client_) {
        client_->fetch_snapshot();
        return true;
    }
    return false;
}

/// @brief Команда help
bool CommandHandlers::Help(CommandArgs&& args) {
    return true;
}

/// @brief Команда exit
bool CommandHandlers::Exit(CommandArgs&& args) {
    return false;
}

/// @brief Конструктор
ArgsParsers::ArgsParsers(Menu& menu, std::istream& input)
    : menu_(menu)
    , input_(input) {
}

/// @brief Парсинг connect
CommandArgs ArgsParsers::ParseConnectArgs(std::istream& input) {
    return CommandArgs{std::in_place_type<NoArgs>};
}

}  // namespace menu
