#include "tcpserver.h"

#include <chrono>
#include <utility>

#include "../../bid_ask_book/src/bid_ask_book.h"
#include "../../snapshots_broadcaster/dom_manager.h"

// Сервер принимает HELLO <client_id>, обновляет client_list и шлет периодические snapshot.
TCPServer::TCPServer(boost::asio::io_context& io_context, unsigned short port) :
    io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), snapshot_timer_(io_context),
    client_list_(std::move(makeClientList())),
    dom_manager_(
        std::make_unique<server::DOMManager>(io_context, *client_list_, std::make_unique<server::OrderBook>())) {}

TCPServer::~TCPServer() {
    StopServer();
}

void TCPServer::StartServer() {
    DoAccept();
    // TODO pass StartBroadcastCmd params in some sort of
    //  ServerCFG struct, now it`s by default
    //        int interval_ms;
    //        bool use_test_broadcast;
    dom_manager_->operator()({server::StartBroadcastCmd{}});
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
        [this](const std::vector<std::uint8_t>& frame, const std::shared_ptr<Session>& s) {
            HandleMessage(frame, s);
        },
        [this](const std::shared_ptr<Session>& s) {
            HandleDisconnect(s);
        });
    session->Start();
    return session;
}

void TCPServer::SendUpdateMessage(const std::string& message) {
    const std::vector<char> payload(message.begin(), message.end());
    client_list_->broadcast_to_subscribed(payload);
}

void TCPServer::HandleMessage(const std::vector<std::uint8_t>& frame, const std::shared_ptr<Session>& session) {
    const ClientId client_id = ParseClientId(frame);
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

// this is the responsibility of DOMManager
void TCPServer::ScheduleSnapshots() {
    // Таймер только задаёт период; байты на сокет уходят в SendUpdateMessage ->
    // IClientList::broadcast_to_subscribed -> Session::SendMsg -> boost::asio::async_write.

    // This function is replaced with DOMManager::start_broadcasting

    /*snapshot_timer_.expires_after(std::chrono::seconds(1));
    auto self = shared_from_this();
    snapshot_timer_.async_wait([this, self](const boost::system::error_code &error) {
        if (error) {
            return;
        }

        static std::uint64_t seq = 0;
        ++seq;
        SendUpdateMessage(
            "SNAPSHOT seq=" + std::to_string(seq) + " best_bid=10100 best_ask=10105 spread=5");
        ScheduleSnapshots();
    });*/
}

ClientId TCPServer::ParseClientId(const std::vector<std::uint8_t>& frame) const {
    if (frame.empty()) {
        return 0;
    }
    std::size_t end = frame.size();
    while (end > 0 && (frame[end - 1] == '\n' || frame[end - 1] == '\r' || frame[end - 1] == ' ')) {
        --end;
    }
    std::size_t beg = 0;
    while (beg < end && frame[beg] == ' ') {
        ++beg;
    }
    if (beg >= end) {
        return 0;
    }
    static constexpr char kHello[] = "HELLO";
    constexpr std::size_t kHelloLen = sizeof(kHello) - 1;
    if (end - beg < kHelloLen + 2) {
        return 0;
    }
    for (std::size_t i = 0; i < kHelloLen; ++i) {
        if (frame[beg + i] != static_cast<std::uint8_t>(kHello[i])) {
            return 0;
        }
    }
    if (frame[beg + kHelloLen] != ' ') {
        return 0;
    }
    ClientId id = 0;
    for (std::size_t i = beg + kHelloLen + 1; i < end; ++i) {
        const auto c = frame[i];
        if (c < '0' || c > '9') {
            return 0;
        }
        id = id * 10 + static_cast<ClientId>(c - '0');
    }
    return id;
}
