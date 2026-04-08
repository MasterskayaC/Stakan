#include <memory>
#include <set>
#include <utility>

#include "client_lib_interface.hpp"

namespace client_lib {

/**
 * @brief Реализация интерфеса IClientCallbacks
 */
class CallbacksAdapter : public tcp_client::IClientCallbacks {
public:
    CallbacksAdapter(ClientCallbacks&& cc) : cc_(std::move(cc)) {}

    ~CallbacksAdapter() = default;

    void OnConnected() override {
        cc_.on_connected();
    }

    void OnDisconnected() override {
        cc_.on_disconnected();
    }

    void OnTopOfBook(const common::Snapshot& snapshot) override {
        cc_.on_snapshot(snapshot);
    }

    void OnError(std::string_view message) override {
        cc_.on_error(ConnectionState::Error, message);
    };

private:
    ClientCallbacks cc_;
};

/**
 * @brief Реализация клиентской библиотеки стакана.
 *
 * Класс реализует интерфейс @ref IOrderBookClient и служит промежуточным
 * адаптером между внешним кодом (CLI / GUI) и TCP-клиентом коллеги.
 */
class OrderBookClient final : public IOrderBookClient {
public:
    /**
     * @brief Создает клиент с дефолтными настройками.
     */
    OrderBookClient(ClientCallbacks&& cc) : callbaсks_(std::move(cc)) {
        client_ = std::make_unique<tcp_client::TcpClient>(config_, &callbaсks_);
    }

    /**
     * @brief Создает клиент с кастомным хостом и портом.
     */
    OrderBookClient(const std::string& host, std::uint16_t port, std::string name, ClientCallbacks&& cc) :
        callbaсks_(std::move(cc)), config_({.host = host, .port = port, .client_name = name}),
        client_(std::make_unique<tcp_client::TcpClient>(config_, &callbaсks_)) {}

    /**
     * @brief Освобождает внутренние ресурсы.
     */
    ~OrderBookClient() override = default;

    OrderBookClient(const OrderBookClient&) = delete;
    OrderBookClient& operator=(const OrderBookClient&) = delete;
    OrderBookClient(OrderBookClient&&) noexcept = default;
    OrderBookClient& operator=(OrderBookClient&&) noexcept = default;

    void Connect() override {
        if (client_->Connect()) {
            state_ = ConnectionState::Connected;
        }
    }

    /**
     * @brief Разрывает текущее соединение.
     */
    void Disconnect() override {
        client_->Disconnect();
        state_ = ConnectionState::Disconnected;
    }

    /**
     * @brief Подписка на инструмент.
     */
    void Subscribe(std::string ticker) override {
        if (client_->Subscribe(ticker)) {
            tickers_.emplace(std::move(ticker));
        }
    }

    /**
     * @brief Отписка от инструмента.
     */
    void Unsubscribe(std::string ticker) override {
        if (client_->Unsubscribe(ticker)) {
            tickers_.erase(std::move(ticker));
        }
    }

    /**
     * @brief Проверяет наличие активного соединения.
     *
     * @return @c true, если внутренний транспорт считает себя подключённым.
     * @return @c false в противном случае.
     */
    bool IsConnected() const noexcept override {
        return client_->IsConnected();
    };

    /**
     * @brief Возвращает текущее состояние клиента.
     *
     * @return Текущее состояние клиентской библиотеки.
     */
    ConnectionState State() const noexcept override {
        return state_;
    };

private:
    std::set<std::string> tickers_;
    CallbacksAdapter callbaсks_;
    tcp_client::ClientConfig config_;
    std::unique_ptr<tcp_client::TcpClient> client_;
    ConnectionState state_ = ConnectionState::Disconnected;
};

std::unique_ptr<IOrderBookClient> MakeConfiguredClient(ClientCallbacks&& cc) {
    return std::make_unique<OrderBookClient>(std::move(cc));
}

std::unique_ptr<IOrderBookClient> MakeConfiguredClient(std::string host,
                                                       uint16_t port,
                                                       std::string name,
                                                       ClientCallbacks&& cc) {
    return std::make_unique<OrderBookClient>(host, port, name, std::move(cc));
}

}  // namespace client_lib
