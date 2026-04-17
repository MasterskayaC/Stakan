#pragma once
#include "session.h"
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>
#include "../client_list/client_list.h"

namespace server {
    class DOMManager;
}

class IClientList;

using boost::asio::ip::tcp;

// Главный TCP-сервер: принимает подключения, отслеживает reconnect и рассылает snapshot.
class TCPServer : public std::enable_shared_from_this<TCPServer> {
public:
    TCPServer(boost::asio::io_context &io_context, unsigned short port);

    ~TCPServer();

    /**
     * @brief Starts accept loop and periodic snapshot scheduling.
     */
    void StartServer();

    /**
     * @brief Stops accept loop and active sessions.
     */
    void StopServer();

    /**
     * @brief Sends snapshot/update payload to subscribed clients.
     * @param message Serialized update payload.
     */
    void SendUpdateMessage(const std::string &message);

private:
    /**
     * @brief Starts asynchronous accept for new TCP connections.
     */
    void DoAccept();

    /**
     * @brief Builds a Session for accepted socket and binds callbacks.
     * @param socket Accepted socket.
     * @param error Result of async_accept.
     */
    std::shared_ptr<Session> OnAccept(std::shared_ptr<tcp::socket> socket,
                                      const boost::system::error_code &error);

    /**
     * @brief Handles incoming frame from one session (HELLO handshake).
     * TODO:Перенес обработку кадров в отдельный модуль протокола.
     */
    void HandleMessage(const std::vector<std::uint8_t> &frame, const std::shared_ptr<Session> &session);

    /**
     * @brief Handles session disconnect and cleans ownership in client_list.
     */
    void HandleDisconnect(const std::shared_ptr<Session> &session);

    /**
     * @brief Arms periodic timer and triggers SendUpdateMessage.
     *
     */
    void ScheduleSnapshots();

    /**
     * @brief Parses HELLO <client_id> from raw frame bytes.
     */
    ClientId ParseClientId(const std::vector<std::uint8_t> &frame) const;

    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;
    boost::asio::steady_timer snapshot_timer_;
    std::unique_ptr<IClientList> client_list_;
    std::unique_ptr<server::DOMManager> dom_manager_;
};
