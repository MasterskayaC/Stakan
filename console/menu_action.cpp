#include "menu_action.h"

namespace menu {

namespace detail {

/** @todo Remove this forward declaration once Snapshot is in the repo */
struct Snapshot;

/** @todo Command argument parsing functions */

void PrintSnapshot(std::ostream& output, Snapshot&& snapshot) {}

}  // namespace detail

CommandHandlers::CommandHandlers(Menu& menu, std::ostream& output, client_lib::IOrderBookClient& client) :
    menu_(menu), output_(output), client_(client) {}

bool CommandHandlers::Buy(Menu::CommandArgs&& args) const {
    return false;
}

bool CommandHandlers::Sell(Menu::CommandArgs&& args) const {
    return false;
}

bool CommandHandlers::Book(Menu::CommandArgs&& args) const {
    return false;
}

bool CommandHandlers::Exit() const {
    return false;
}

bool CommandHandlers::Help() const {
    return false;
}

ArgsParsers::ArgsParsers(Menu& menu, std::istream& input) : menu_(menu), input_(input) {}

Menu::CommandArgs ArgsParsers::ParseBuyArgs(std::istream&) {
    return {};
}

Menu::CommandArgs ArgsParsers::ParseSellArgs(std::istream&) {
    return {};
}

Menu::CommandArgs ArgsParsers::ParseBookArgs(std::istream&) {
    return {};
}

}  // namespace menu
