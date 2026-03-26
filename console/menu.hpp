#pragma once

#include <deque>
#include <functional>
#include <iosfwd>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace menu {
/**
 * @brief Command Line Interface (CLI) for Order Management.
 * 
 * Supporting commands:
 * - `buy  <qty> <price>`  - Place a buy order.
 * - `sell <qty> <price>`  - Place a sell order.
 * - `book <update(ms)>`   - Show best bid/ask.
 * - `exit`                - Exit application.
 * - `help`                - Show this help message.
 * 
 * @code
 * Example session:
 *     Best Bid: 10.5 (100)
 *     Best Ask: 10.5 (100)
 *   > buy qty 100 price 10.5
 *   > book update 100
 * @endcode
 */

class Menu final {
public:
    using NoArgs = std::monostate;
    using CommandArgs = std::variant<NoArgs>;
    using CommandHandler = std::function<bool(CommandArgs&)>;
    using ArgParser = std::function<CommandArgs(std::istream&)>;

    Menu(std::istream& input, std::ostream& output);

    /**
     * @brief The method adds a command in a minimal form
     * @param comand      Command name.
     * @param description Brief description of the command.
     * @param handler     Handler function for the command.
     */
    void AddCommand(std::string command, 
                    std::string description,
                    CommandHandler handler);
    
    /**
     * @brief The method adds command arguments and their parser.
     * @param command     Command name.
     * @param args        Set of command arguments.
     * @param handler     Parser function for the args.
     */
    void AddArgsParser(std::string command, 
                       std::set<std::string>&& args, 
                       ArgParser parser);

    /**
     * @brief The method starts processing commands.
     */
    void Run();

private:
    struct Command {
        std::string command;
        std::set<std::string> args;
        std::string description;
        ArgParser parser;
        CommandHandler handler;
    };

    /**
     * @brief The method adds a command and associated action.
     * @param input Start of user input.
     */
    [[nodiscard]] bool ParseComand(std::istream& input);

private:
    using CommandPtr = std::shared_ptr<Command>;
    using CommandDeque = std::deque<CommandPtr>;
    using CommandHashTable = std::unordered_map<std::string_view, CommandPtr>;

    std::istream& input_;
    std::ostream& output_;
    CommandDeque comands_;
    CommandHashTable name_to_commands_;
};
    
} // namespace menu
