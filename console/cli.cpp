#include "cli.hpp"

namespace cli {

namespace detail {

struct BestBidAsk {
    std::vector<int> bid;
    std::vector<int> ask;
};

void PrintBestBidAndAsk(std::ostream& output, BestBidAsk&& bid_ask) {}
    
} // namespace detail

CommandHandlers::CommandHandlers(menu::Menu& menu, /*net::Client& client*/
                   std::istream& input, std::ostream& output)
    : menu_(menu)
    /*client_(client)*/
    , input_(input)
    , output_(output)
    {}

bool CommandHandlers::Book(std::istream& input) const {
    return false;
}

bool CommandHandlers::Exit() const {
    return false;
}

bool CommandHandlers::Buy(std::istream& input) const {
    return false;
}

bool CommandHandlers::Sell(std::istream& input) const {
    return false;
}

} // namespace cli