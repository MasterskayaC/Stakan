#pragma once

#include <iosfwd>

#include "menu.hpp"

namespace cli {

class CommandHandlers {
public:
    CommandHandlers(menu::Menu& menu, /*net::Client& client*/
             std::istream& input, std::ostream& output);

private:
    bool Buy(std::istream& input) const;
    bool Sell(std::istream& input) const;
    bool Book(std::istream& input) const;
    bool Exit() const;

private:
    menu::Menu& menu_;
    /*net::Client& client_;*/
    std::istream& input_;
    std::ostream& output_;
};

} // namespace cli