#pragma once

/** @todo Uncomment after client_lib is added to the repo */
// #include <client_lib/client_lib_interface.hpp>

namespace app {

/** @todo remove the namespace when client_lib_interface.hpp appears in the repo*/
namespace client_lib {

struct IOrderBookClient {};

} // namespace client_lib

class Application final {
public:
    /**
     * @brief The constructor establishes a connection to the server.
     */
    explicit Application();

    /**
     * @brief The method starts the application.
     */
    void Run();

private:
    client_lib::IOrderBookClient client_;
};

} // namespace app
