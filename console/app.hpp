#pragma once

/** @todo Uncomment after client_lib_interface.hpp is added to the repo */
// #include "client_lib_interface.hpp"

namespace app {

/** @todo remove the namespace when client_lib_interface.hpp appears in the repo*/
namespace client_lib {

struct ClientConfig {};
struct IOrderBookClient {};

} // namespace client_lib

class Application {
public:
    explicit Application(client_lib::ClientConfig&&);

    void Run();

private:
    client_lib::IOrderBookClient client_;
};

} // namespace app