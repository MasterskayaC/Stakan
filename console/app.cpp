#include <iostream>
#include <string>

#include "app.h"
#include "menu.h"
#include "menu_action.h"

namespace app {

Application::Application()
    : order_book_client_(
          nullptr) // TODO: use MakeConfiguredClient() after PR #41 merge
      ,
      snapshot_client_(std::make_shared<console::SnapshotConsoleClient>(
          order_book_client_)) {
  menu_.reset(new menu::Menu(std::cin, std::cout, snapshot_client_));
  command_handlers_.reset(
      new menu::CommandHandlers(*menu_, std::cout, snapshot_client_));

  snapshot_client_->set_snapshot_callback(
      [this](const client_lib::Snapshot &snap) { on_snapshot_received(snap); });
  snapshot_client_->set_error_callback(
      [this](const std::string &err) { on_error(err); });
}

Application::~Application() = default;

void Application::Run() {
  std::cout << "Snapshot Console Client\n";
  menu_->PrintHelp();
  menu_->Run();
}

bool Application::connect(const std::string &host, uint16_t port) {
  snapshot_client_->connect_to_server(host, port);
  if (snapshot_client_->is_connected()) {
    std::cout << "Connected to " << host << ":" << port << "\n";
    return true;
  }
  std::cerr << "Failed to connect to " << host << ":" << port << "\n";
  return false;
}

bool Application::fetch_snapshot() {
  if (!snapshot_client_->is_connected()) {
    std::cerr << "Not connected to server\n";
    return false;
  }
  snapshot_client_->fetch_snapshot();
  return true;
}

void Application::on_snapshot_received(const client_lib::Snapshot &snapshot) {
  std::cout << "=== Snapshot received ===\n";
  std::cout << "ЗАГЛУШКААААААААААААА\n";
}

void Application::on_error(const std::string &error) {
  std::cerr << "ERROR: " << error << "\n";
}

} // namespace app
