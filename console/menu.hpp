#pragma once

#include <functional>
#include <iosfwd>
#include <map>
#include <set>
#include <string>

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
 *   > buy qty 100 price 10.5
 *   > book update 100
 *     Best Bid: 10.5 (100)
 *     Best Ask: 10.5 (100)
 * @endcode
 */

class Menu {
public:
    using Handler = std::function<bool(std::istream&)>;

    Menu(std::istream& input, std::ostream& output);

    void AddAction(std::string comand, 
                   std::set<std::string> args, 
                   std::string description, 
                   Handler handler);
    void Run();

private:
    struct Action {
        Handler handler;
        std::set<std::string> args;
        std::string description;
    };

    [[nodiscard]] bool ParseComand(std::istream& input);

private:
    std::istream& input_;
    std::ostream& output_;
    std::map<std::string, Action> actions_;
};
    
} // namespace menu