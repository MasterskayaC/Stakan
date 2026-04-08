#pragma once

#include <iosfwd>

/** @todo Uncomment after client_lib is added to the repo*/
// #include <client_lib/client_lib_interface.hpp>
#include "menu.hpp"

namespace menu {

/** @todo remove the namespace when client_lib_interface.hpp appears in the repo*/
namespace client_lib {

struct IOrderBookClient {};

} // namespace client_lib

/**
 * @brief Class of console interface command handlers.
 * 
 * Creates handlers for menu commands.
 */
class CommandHandlers final {
public:
    /**
     * @brief The constructor initializes dependencies for command processing.
     * 
     * @note Registration logic example (pseudocode):
     * @code
     * menu_.AddCommand("Buy", "Place a buy order",
     *                  std::bind(&CommandHandlers::Buy, this, std::placeholders::_1));
     * @endcode
     * 
     * @param menu ref for registering a handler for a command.
     * @param output ref to the output stream of the work results.
     * @param client ref to the interface for interaction with the TСP-client.
     */
    CommandHandlers(Menu& menu,
                    std::ostream& output,
                    client_lib::IOrderBookClient& client);

private:
    /**
     * @name Сommand handlers.
     * A list of methods for processing the command.
     * @return true, if the command was executed correctly; false - not correct.
     * @{
     */
    bool Buy(Menu::CommandArgs&& args) const;
    bool Sell(Menu::CommandArgs&& args) const;
    bool Book(Menu::CommandArgs&& args) const;
    bool Exit() const;
    bool Help() const;
    /** @} */

private:
    Menu& menu_;
    std::ostream& output_;
    client_lib::IOrderBookClient& client_;
};

class ArgsParsers final {
public:
    /**
     * @brief The constructor initializes dependencies for processing command arguments.
     * 
     * @note Registration logic example (pseudocode):
     * @code
     * menu_.AddArgsParser("Buy", {"qty", "price"},
     *                     std::bind(&CommandParsers::ParseBuyArgs, this, std::placeholders::_1));
     * @endcode
     * 
     * @param menu ref for registering a handler for a command.
     * @param input ref to the input stream for processing command arguments.
     */
    ArgsParsers(menu::Menu& menu, std::istream& input);

private:
    /**
     * @name Args Parsers.
     * A list of methods for parsing command arguments.
     * @return Menu::CommandArgs An object containing the extracted command arguments.
     * @{
     */
    Menu::CommandArgs ParseBuyArgs(std::istream&);
    Menu::CommandArgs ParseSellArgs(std::istream&);
    Menu::CommandArgs ParseBookArgs(std::istream&);
    /** @} */

private:
    Menu& menu_;
    std::istream& input_;
};

} // namespace menu
