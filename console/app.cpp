#include <iostream>

#include "app.hpp"
#include "cli.hpp"
#include "menu.hpp"

namespace app {

Application::Application(client_lib::ClientConfig&& conect) {}

void Application::Run() {
    menu::Menu menu { std::cin, std::cout };
    /** @todo Uncomment after client_lib_interface.hpp is added to the repo */
    //cli::CommandHandlers ch{ menu, std::cin, std::cout, , client_ };
    menu.Run();
}

} // namespace app