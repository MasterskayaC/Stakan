#include "session.h"
#include "../../client_list/client_list.h"

#include <cstdint>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>

using boost::asio::ip::tcp;

// Главный TCP-сервер: принимает подключения, отслеживает reconnect и рассылает snapshot.
class TCPServer :public std::enable_shared_from_this<TCPServer> {
public:
    TCPServer(boost::asio::io_context& io_context, unsigned short port);
    ~TCPServer();
    /**
     *  @brief Init socket and  do_accept
    */
    void StartServer();

    /**
     *  @brief Close acceptor
    */
    void StopServer();

    /**
     *  @brief Aend update msg to all client
     *  @param Update message
    */
    void SendUpdateMessage(const std::string& message);

private:

    /**
     *  @brief Init async waiting new connection
    */
    void DoAccept();

    /**
     *  @brief Create socket ptr, set session to sessions container
     *  @param Socket
     *  @param Error object
    */
    std::shared_ptr<Session> OnAccept(std::shared_ptr<tcp::socket> socket,
                   const boost::system::error_code& error);
    void HandleMessage(const std::vector<std::uint8_t>& frame, const std::shared_ptr<Session>& session);
    void HandleDisconnect(const std::shared_ptr<Session>& session);
    void ScheduleSnapshots();
    ClientId ParseClientId(const std::vector<std::uint8_t>& frame) const;

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    boost::asio::steady_timer snapshot_timer_;
    std::unique_ptr<IClientList> client_list_;
};
