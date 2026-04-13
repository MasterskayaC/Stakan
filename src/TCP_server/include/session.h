#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

// Обертка над одним TCP-клиентом: чтение кадров как байтов и очередь отправки через async_write.
class Session : public std::enable_shared_from_this<Session> {
public:
    using OnDataCallback = std::function<void(const std::vector<std::uint8_t>&, const std::shared_ptr<Session>&)>;
    using OnDisconnectCallback = std::function<void(const std::shared_ptr<Session>&)>;

    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
 /**
     *  @brief Start async reading
        Comment view    
        */
    void Start();
/**
     *  @brief Close socket and stop all operations
    */
    void Stop();
 /**
     *  @brief Push msg to messages queue and start writing to stream
     *  @param Message
    */
    /// Enqueues bytes; actual socket I/O is performed by @c boost::asio::async_write in Write().
    void SendMsg(const std::vector<char>& message);
    bool IsOpen() const;
    void SetCallbacks(OnDataCallback on_data, OnDisconnectCallback on_disconnect);

private:
/**
     *  @brief Async reading from socket
    */
    void Read();
 /**
     *  @brief Check error and read "bytes_transferred" bytes
     *  @param Error object
     *  @param Numbers reading bytes
    */
    void ProcessRead(const boost::system::error_code& error, std::size_t bytes_transferred);
/**
     *  @brief Async writing to stream
    */
    void Write();
/**
     *  @brief Check error and write "bytes_transferred" bytes
     *  @param Error object
     *  @param Numbers bytes to writing
    */
    void ProcessWrite(const boost::system::error_code& error, std::size_t bytes_transferred);
/**
     *  @brief Serialize and send msg
     *  @param Order or update struct
    */

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    boost::asio::streambuf read_buffer_;
    std::deque<std::vector<char>> messages_queue_;
    std::mutex write_mutex_;
    OnDataCallback on_data_;
    OnDisconnectCallback on_disconnect_;
};
