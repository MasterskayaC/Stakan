#include "menu.h"

#include <algorithm>
#include <iostream>

#include "menu_action.h"

namespace menu {

Menu::Menu(std::istream& input, std::ostream& output, std::shared_ptr<console::SnapshotConsoleClient> client)
    : input_(input)
    , output_(output)
    , client_(std::move(client)) {
}

void Menu::RegisterHandler(const std::string& command, const std::string& description, CommandHandler handler) {
    auto cmd_ptr = std::make_shared<Command>(Command{command, description, std::move(handler)});
    commands_.emplace(command, cmd_ptr);
}

void Menu::Run() {
    running_ = true;
    while (running_) {
        output_ << "> ";
        if (!ParseCommand(input_)) {
            break;
        }
    }
}

void Menu::Stop() {
    running_ = false;
}

bool Menu::ParseCommand(std::istream& input) {
    std::string cmd;
    if (!(input >> cmd)) {
        return false;
    }

    auto it = commands_.find(cmd);
    if (it == commands_.end()) {
        if (cmd == "help") {
            PrintHelp();
            return true;
        }
        output_ << "Unknown command: " << cmd << "\n";
        return true;
    }

    CommandArgs args{std::in_place_type<NoArgs>};
    return it->second->handler(args);
}

void Menu::PrintHelp() {
    output_ << "Available commands:\n";
    for (const auto& [name, cmd] : commands_) {
        output_ << "  " << cmd->command << " - " << cmd->description << "\n";
    }
}

}  // namespace menu
