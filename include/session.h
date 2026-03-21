#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>
#include <queue>

struct OrderBinaryMessage { //struct for de/serealization order
    uint8_t  messageType;
    uint32_t price;
    uint32_t quantity;
    uint64_t orderId;

    OrderBinaryMessage() {}
    OrderBinaryMessage(uint8_t mt, uint32_t p, uint32_t q, uint64_t oid)
        : messageType(mt), price(p), quantity(q), orderId(oid) {
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& messageType;
        ar& price;
        ar& quantity;
        ar& orderId;
    }
};

struct UpdateBinaryMessage { //struct for de/serealization update
    uint32_t bid_price;
    uint32_t bid_quantity;
    uint32_t ask_price;
    uint32_t ask_quantity;

    UpdateBinaryMessage() {}
    UpdateBinaryMessage(uint32_t b_p, uint32_t b_q, uint32_t a_p, uint32_t a_q)
        : bid_price(b_p), bid_quantity(b_q), ask_price(a_p), ask_quantity(a_q) {
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& bid_price;
        ar& bid_quantity;
        ar& ask_price;
        ar& ask_quantity;
    }
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void Start();         //start server
    void Stop();          //stop server
    void SendMsg(const std::string& message); //push msg to queue and start writing to stream

private:
    void Read(); //async reading from socket
    void ProcessRead(const boost::system::error_code& error, size_t bytes_transferred); //check error and read "bytes_transferred" bytes
    void Write(); //async writing to stream
    void ProcessWrite(const boost::system::error_code& error, size_t bytes_transferred);//check error and write "bytes_transferred" bytes

    template <typename StructMsg>
    void SendBinaryMessage(const StructMsg& msg) { //serialize and send msg
        std::ostringstream archive_stream;
        boost::archive::binary_oarchive archive(archive_stream);
        archive << msg;

        SendMsg(archive_stream.str());
    }

    template <typename StructMsg>
    void ReadBinaryMessage(const StructMsg& msg) { // not finish deserialize and read msg

    }

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::array<char, 1024> read_buffer_; // ??????
    std::queue<std::string> messages_queue_;
    std::mutex write_mutex_;
    bool is_writing_ = false;

};



