#include <iostream>
#include <string>

#include "app.h"

namespace app {

Application::Application()
    : running_(false) {
}

Application::~Application() {
    stop_updates();
}

void Application::Run() {
    std::cout << "Snapshot Console Client\n";
    print_help();

    running_ = true;
    while (running_) {
        std::cout << "> ";
        std::string cmd;
        if (!(std::cin >> cmd)) {
            break;
        }

        if (cmd == "connect") {
            std::string host;
            uint16_t port;
            std::cin >> host >> port;
            connect(host, port);
        } else if (cmd == "snapshot") {
            fetch_snapshot();
        } else if (cmd == "subscribe") {
            start_updates();
        } else if (cmd == "unsubscribe") {
            stop_updates();
        } else if (cmd == "help") {
            print_help();
        } else if (cmd == "exit") {
            running_ = false;
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
            print_help();
        }
    }
}

void Application::print_help() {
    std::cout << "Commands:\n";
    std::cout << "  connect <host> <port> - Connect to server\n";
    std::cout << "  snapshot               - Request snapshot\n";
    std::cout << "  subscribe              - Start receiving updates\n";
    std::cout << "  unsubscribe            - Stop receiving updates\n";
    std::cout << "  help                   - Show this help\n";
    std::cout << "  exit                   - Exit\n";
}

bool Application::connect(const std::string& host, uint16_t port) {
    // заглушка
    return true;
}

bool Application::fetch_snapshot() {
    // заглушка
    return true;
}

void Application::start_updates() {
    // заглушка
}

void Application::stop_updates() {
    // заглушка
}

} 
