#include "app.hpp"

#include <utility>

int main() {
    app::ConectionData data;

    app::Application app(
        std::move(data)
    );
    app.Run();
}