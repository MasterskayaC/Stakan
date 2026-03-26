#include "menu.hpp"

namespace menu {

Menu::Menu(std::istream& input, std::ostream& output)
    : input_(input)
    , output_(output) {}

void Menu::AddCommand(const std::string command, 
                    const std::string description,
                    Menu::CommandHandler handler) {}

void Menu::AddArgsParser(std::string command,
                         std::set<std::string>&& args, 
                         Menu::ArgParser parser) {}

void Menu::Run() {}

[[nodiscard]] bool Menu::ParseComand(std::istream& input) { 
    return false; 
}

} // namespace menu
