#include "../../include/tcp_client/client.hpp"
#include "../../include/tcp_client/protocol.hpp"
#include "../../bid_ask_snaphot_interface/bid_ask_interface.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

namespace {

    class ConsoleCallbacks final : public tcp_client::IClientCallbacks {
    public:
        void OnConnected() override {
            std::cout << "Connected to server\n";
        }

        void OnDisconnected() override {
            std::cout << "Disconnected from server\n";
        }

        void OnTopOfBook(const common::Snapshot& snapshot) override {
            std::cout << "=== Snapshot received ===\n";
            for (size_t i = 0; i < common::topN; ++i) {
                const auto& bid = snapshot.topBids[i];
                const auto& ask = snapshot.topAsks[i];
                if (bid.id != 0) {
                    std::cout << "Bid:  Price=" << bid.price
                              << " Qty=" << bid.quantity << '\n';
                }
                if (ask.id != 0) {
                    std::cout << "Ask:  Price=" << ask.price
                              << " Qty=" << ask.quantity << '\n';
                }
            }
            std::cout << "========================\n";
        }

        void OnError(std::string_view message) override {
            std::cerr << "Error: " << message << '\n';
        }
    };

}  // namespace

int main() {
    ConsoleCallbacks callbacks{};
    tcp_client::ClientConfig config{};
    tcp_client::TcpClient client{config, &callbacks};

    if (!client.Connect()) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    // Для демонстрации подписка на инструмент
    std::string symbol = "EURUSD";
    if (!client.Subscribe(symbol)) {
        std::cerr << "Failed to subscribe to " << symbol << '\n';
    } else {
        std::cout << "Subscribed to " << symbol << "\n";
    }

    // Даем время на получение снапшотов
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Отписка и завершение
    client.Unsubscribe(symbol);
    client.Disconnect();

    return 0;
}
