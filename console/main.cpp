#include "app.hpp"

#include <utility>

int main() {
    app::client_lib::ClientConfig conect;

    app::Application app(
        std::move(conect)
    );
    app.Run();
}