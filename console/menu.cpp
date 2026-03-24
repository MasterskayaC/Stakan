#include "menu.hpp"

namespace menu {

Menu::Menu(std::istream& input, std::ostream& output)
    : input_(input)
    , output_(output) {}

void Menu::AddAction(std::string comand, 
                     std::set<std::string> args, 
                     std::string description, 
                     Handler handler) {}

void Menu::Run() {}

[[nodiscard]] bool Menu::ParseComand(std::istream& input) { 
    return false; 
}

} // namespace menu