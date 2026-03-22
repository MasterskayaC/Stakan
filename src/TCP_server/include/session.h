#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>
#include <queue>

/**
 *  @brief Interface session/ It's the object of a specific connection
*/
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    /**
     *  @brief Start asynk reading
    */
    void Start();

    /**
     *  @brief Close socet and stop all operations
    */
    void Stop();

    /**
     *  @brief Push msg to mesaages queue and start writing to stream
     *  @param Message
    */
    void SendMsg(const std::string& message);

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
    void ProcessRead(const boost::system::error_code& error, size_t bytes_transferred);

    /**
     *  @brief Async writing to stream
    */
    void Write();

    /**
     *  @brief Check error and write "bytes_transferred" bytes
     *  @param Error object
     *  @param Numbers bytes to writing
    */
    void ProcessWrite(const boost::system::error_code& error, size_t bytes_transferred);

    /**
     *  @brief Serialize and send msg
     *  @param Order or update struct
    */
    template <typename StructMsg>
    void SendBinaryMessage(const StructMsg& msg) { //

    }

    /**
     *  @brief deserialize and read msg
     *  @param Order or update struct
    */
    template <typename StructMsg>
    void ReadBinaryMessage(const StructMsg& msg) {

    }

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::array<char, 1024> read_buffer_; // need to rework
    std::queue<std::string> messages_queue_;
    std::mutex write_mutex_;
    bool is_writing_ = false;

};



