#include "tcp_client/client.hpp"

#include <iostream>
#include <string_view>

namespace {

class ConsoleCallbacks final : public tcp_client::IClientCallbacks {
public:
    void OnConnected() override {
        std::cout << "Connected\n";
    }

    void OnDisconnected() override {
        std::cout << "Disconnected\n";
    }

    void OnTopOfBook(const tcp_client::TopOfBook& update) override {
        std::cout << "TOB " << update.symbol
                  << " bid=" << update.best_bid.price
                  << " ask=" << update.best_ask.price << '\n';
    }

    void OnError(std::string_view message) override {
        std::cout << "Error: " << message << '\n';
    }
};

}  // namespace

int main() {
    ConsoleCallbacks callbacks{};
    tcp_client::ClientConfig config{};
    tcp_client::TcpClient client{config, &callbacks};

    // Здесь будет полноценный жизненный цикл приложения (CLI/UI цикл событий).
    if (!client.Connect()) {
        std::cout << "Connect is not implemented yet\n";
    }

    return 0;
}
