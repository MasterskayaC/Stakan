#include "session.h"
#include "../../client_list/client_list.h"

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>

using boost::asio::ip::tcp;

// Главный TCP-сервер: принимает подключения, отслеживает reconnect и рассылает snapshot.
class TCPServer :public std::enable_shared_from_this<TCPServer> {
public:
    TCPServer(boost::asio::io_context& io_context, unsigned short port);
    ~TCPServer();
    void StartServer();
    void StopServer();
    void SendUpdateMessage(const std::string& message);

private:
    void DoAccept();
    std::shared_ptr<Session> OnAccept(std::shared_ptr<tcp::socket> socket,
                   const boost::system::error_code& error);
    void HandleMessage(const std::string& line, const std::shared_ptr<Session>& session);
    void HandleDisconnect(const std::shared_ptr<Session>& session);
    void ScheduleSnapshots();
    ClientId ParseClientId(const std::string& line) const;

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    boost::asio::steady_timer snapshot_timer_;
    std::unique_ptr<IClientList> client_list_;
    mutable std::mutex sessions_mutex_;
    std::unordered_map<Session*, ClientId> session_to_client_;
};
