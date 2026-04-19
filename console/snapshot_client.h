#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "client_lib_interface.hpp"

namespace console {

using SnapshotCallback = std::function<void(const client_lib::Snapshot &)>;
using ErrorCallback = std::function<void(const std::string &)>;

class SnapshotConsoleClient {
public:
  explicit SnapshotConsoleClient(
      std::shared_ptr<client_lib::IOrderBookClient> client);
  ~SnapshotConsoleClient();

  void set_snapshot_callback(SnapshotCallback callback);
  void set_error_callback(ErrorCallback callback);

  void connect_to_server(const std::string &host, uint16_t port);
  void disconnect_from_server();
  void fetch_snapshot();
  bool is_connected() const;

private:
  void on_connected();
  void on_disconnected();
  void on_snapshot(const client_lib::Snapshot &snapshot);
  void on_error(std::string_view message);

private:
  std::shared_ptr<client_lib::IOrderBookClient> client_;
  SnapshotCallback snapshot_callback_;
  ErrorCallback error_callback_;
  bool connected_ = false;
};

} // namespace console
