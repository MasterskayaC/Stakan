#include "session.h"

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <mutex>

using boost::asio::ip::tcp;

/**
 *  @brief Interface tcp server
*/
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

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    mutable std::mutex sessions_mutex_;
};
