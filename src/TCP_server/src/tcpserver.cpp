#include "../include/tcpserver.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>

// Сервер принимает HELLO <client_id>, обновляет client_list и шлет периодические snapshot.
TCPServer::TCPServer(boost::asio::io_context& io_context, unsigned short port)
    : io_context_(io_context)
    , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    , snapshot_timer_(io_context)
    , client_list_(makeClientList()) {
}

TCPServer::~TCPServer() {
    StopServer();
}

void TCPServer::StartServer() {
    DoAccept();
    ScheduleSnapshots();
}

void TCPServer::StopServer() {
    boost::system::error_code ec;
    snapshot_timer_.cancel();
    acceptor_.close(ec);

    for (const auto& session : client_list_->get_all_sessions()) {
        if (session) {
            session->Stop();
        }
    }
}

void TCPServer::DoAccept() {
    auto socket = std::make_shared<tcp::socket>(io_context_);
    auto self = shared_from_this();
    acceptor_.async_accept(*socket, [this, self, socket](const boost::system::error_code& error) {
        OnAccept(socket, error);
        if (acceptor_.is_open()) {
            DoAccept();
        }
    });
}

std::shared_ptr<Session> TCPServer::OnAccept(std::shared_ptr<tcp::socket> socket,
                          const boost::system::error_code& error) {
    if (error) {
        return {};
    }

    auto session = std::make_shared<Session>(std::move(socket));
    session->SetCallbacks(
        [this](const std::vector<char>& message, const std::shared_ptr<Session>& s) { HandleMessage(message, s); },
        [this](const std::shared_ptr<Session>& s) { HandleDisconnect(s); });
    session->Start();
    return session;
}

void TCPServer::SendUpdateMessage(const std::string& message) {
    const std::vector<char> payload(message.begin(), message.end());
    client_list_->broadcast_to_subscribed(payload);
}

void TCPServer::HandleMessage(const std::vector<char>& message, const std::shared_ptr<Session>& session) {
    const ClientId client_id = ParseClientId(message);
    if (client_id == 0) {
        const std::string response = "ERR expected `HELLO <client_id>`\n";
        session->SendMsg(std::vector<char>(response.begin(), response.end()));
        return;
    }

    auto old_session = client_list_->get_session(client_id);
    client_list_->add_session(client_id, session);
    client_list_->subscribe(client_id);

    if (old_session && old_session.get() != session.get()) {
        const std::string response = "INFO disconnected: reconnected from another socket\n";
        old_session->SendMsg(std::vector<char>(response.begin(), response.end()));
        old_session->Stop();
    }

    const std::string ok_response = "OK HELLO " + std::to_string(client_id) + "\n";
    session->SendMsg(std::vector<char>(ok_response.begin(), ok_response.end()));
}

void TCPServer::HandleDisconnect(const std::shared_ptr<Session>& session) {
    const auto client_id = client_list_->find_client_id_by_session(session.get());
    if (!client_id.has_value()) {
        return;
    }

    auto current = client_list_->get_session(*client_id);
    if (current && current.get() == session.get()) {
        client_list_->remove_session(*client_id);
    }
}

void TCPServer::ScheduleSnapshots() {
    snapshot_timer_.expires_after(std::chrono::seconds(1));
    auto self = shared_from_this();
    snapshot_timer_.async_wait([this, self](const boost::system::error_code& error) {
        if (error) {
            return;
        }

        static std::uint64_t seq = 0;
        ++seq;
        SendUpdateMessage(
            "SNAPSHOT seq=" + std::to_string(seq) + " best_bid=10100 best_ask=10105 spread=5");
        ScheduleSnapshots();
    });
}

ClientId TCPServer::ParseClientId(const std::vector<char>& message) const {
    std::string line(message.begin(), message.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

    std::istringstream iss(line);
    std::string cmd;
    ClientId id = 0;
    if (!(iss >> cmd >> id)) {
        return 0;
    }
    if (cmd != "HELLO") {
        return 0;
    }
    return id;
}
