#include "cli.hpp"

namespace cli {

namespace detail {

/** @todo Remove this forward declaration once Snapshot is in the repo */
struct Snapshot;

void PrintSnapshot(std::ostream& output, Snapshot&& snapshot) {}
    
} // namespace detail


CommandHandlers::CommandHandlers(menu::Menu& menu,
                                 std::istream& input,
                                 std::ostream& output,
                                 client_lib::IOrderBookClient& client)
    : menu_(menu),
        input_(input),
        output_(output),
        client_(client) {}

bool CommandHandlers::Buy(std::istream& input) const {
    return false;
}

bool CommandHandlers::Sell(std::istream& input) const {
    return false;
}

bool CommandHandlers::Book(std::istream& input) const {
    return false;
}

bool CommandHandlers::Exit() const {
    return false;
}

bool CommandHandlers::Help() const {
    return false;
}

} // namespace cli