#pragma once

#include <iosfwd>

/** @todo Uncomment after client_lib_interface.hpp is added to the repo */
// #include "client_lib_interface.hpp"
#include "menu.hpp"

namespace cli {

/** @todo remove the namespace when client_lib_interface.hpp appears in the repo*/
namespace client_lib {

struct ClientConfig {};
struct IOrderBookClient {};

} // namespace client_lib

class CommandHandlers {
public:
    CommandHandlers(menu::Menu& menu,
                    std::istream& input,
                    std::ostream& output,
                    client_lib::IOrderBookClient& client);

private:
    bool Buy(std::istream& input) const;
    bool Sell(std::istream& input) const;
    bool Book(std::istream& input) const;
    bool Exit() const;
    bool Help() const;

private:
    menu::Menu& menu_;
    std::istream& input_;
    std::ostream& output_;
    client_lib::IOrderBookClient& client_;
};

} // namespace cli