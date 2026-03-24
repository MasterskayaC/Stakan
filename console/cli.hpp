#pragma once

#include <iosfwd>

/** @todo Uncomment after client_lib_interface.hpp is added to the repo */
// #include "../client_lib/client_lib_interface.hpp"
#include "menu.hpp"

namespace cli {

/** @todo remove the namespace when client_lib_interface.hpp appears in the repo*/
namespace client_lib {

struct ClientConfig {};
struct IOrderBookClient {};

} // namespace client_lib

/**
 * @brief Class of console interface command handlers
 * 
 * Creates handlers for menu commands
 */
class CommandHandlers final {
public:
    /**
     * @brief The constructor initializes dependencies for command processing
     * 
     * @note Registration logic example (pseudocode):
     * @code
     * menu_.AddAction("Buy", {"qty", "price"}, "Place a buy order", 
     *                 std::bind(&CommandHandlers::Buy, this, std::placeholders::_1));
     * @endcode
     * 
     * @param menu ref for registering a handler for a command
     * @param input ref to the input stream for processing command arguments
     * @param output ref to the output stream of the work results
     * @param client ref to the interface for interaction with the TСP-client
     */
    CommandHandlers(menu::Menu& menu,
                    std::istream& input,
                    std::ostream& output,
                    client_lib::IOrderBookClient& client);

private:
    /**
     * @name Сommand handlers
     * A list of methods for handling specific user actions
     * @return true, if the command was executed correctly; false - not correct
     * @{
     */
    bool Buy(std::istream& input) const;
    bool Sell(std::istream& input) const;
    bool Book(std::istream& input) const;
    bool Exit() const;
    bool Help() const;
    /** @} */

private:
    menu::Menu& menu_;
    std::istream& input_;
    std::ostream& output_;
    client_lib::IOrderBookClient& client_;
};

} // namespace cli