#pragma once

//#include "client_lib"

namespace app {

struct ConectionData {
    // Something for the connection address or user/server data ...
};

class Application {
public:
    explicit Application(ConectionData&&);
    void Run();

private:
    //net::Client client_;
};

} // namespace app