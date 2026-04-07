#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

// Обертка над одним TCP-клиентом: чтение команд и отправка бинарных сообщений.
class Session : public std::enable_shared_from_this<Session> {
public:
    using OnDataCallback = std::function<void(const std::vector<char>&, const std::shared_ptr<Session>&)>;
    using OnDisconnectCallback = std::function<void(const std::shared_ptr<Session>&)>;

    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void Start();
    void Stop();
    void SendMsg(const std::vector<char>& message);
    bool IsOpen() const;
    void SetCallbacks(OnDataCallback on_data, OnDisconnectCallback on_disconnect);

private:
    void Read();
    void ProcessRead(const boost::system::error_code& error, std::size_t bytes_transferred);
    void Write();
    void ProcessWrite(const boost::system::error_code& error, std::size_t bytes_transferred);

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    boost::asio::streambuf read_buffer_;
    std::deque<std::vector<char>> messages_queue_;
    std::mutex write_mutex_;
    OnDataCallback on_data_;
    OnDisconnectCallback on_disconnect_;
};
