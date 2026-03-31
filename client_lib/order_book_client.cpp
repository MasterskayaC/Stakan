#include <memory>
#include <set>
#include <utility>

#include "client_lib_interface.hpp"
/** @todo Разобраться с включением TCP-client */
#include "../include/tcp_client/client.hpp"

namespace client_lib {

/**
 * @brief Преобразует TopOfBook в формат Snapshot для UI.
 */
Snapshot ToSnapshot(const tcp_client::TopOfBook& top) {
    Snapshot snapshot;

    snapshot.bids.reserve(1);
    snapshot.asks.reserve(1);

    snapshot.bids.push_back(
        {.price = static_cast<double>(top.best_bid.price), .quantity = static_cast<double>(top.best_bid.quantity)});
    snapshot.asks.push_back(
        {.price = static_cast<double>(top.best_ask.price), .quantity = static_cast<double>(top.best_ask.quantity)});

    snapshot.exchange_timestamp_ns = top.exchange_timestamp_ns;

    return snapshot;
}

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

    void OnTopOfBook(const tcp_client::TopOfBook& update) override {
        cc_.on_snapshot(ToSnapshot(update));
    }

    void OnError(std::string_view message) override {};

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
        callbaсks_(std::move(cc)), config_({.host = host, .port = port, .client_name = name}) {
        client_ = std::make_unique<tcp_client::TcpClient>(config_, &callbaсks_);
    }

    /**
     * @brief Освобождает внутренние ресурсы.
     */
    ~OrderBookClient() override = default;

    OrderBookClient(const OrderBookClient&) = delete;
    OrderBookClient& operator=(const OrderBookClient&) = delete;
    OrderBookClient(OrderBookClient&&) noexcept = default;
    OrderBookClient& operator=(OrderBookClient&&) noexcept = default;

    void Connect() override {
        client_->Connect();
    }

    /**
     * @brief Разрывает текущее соединение.
     */
    void Disconnect() override {
        if (IsConnected()) {
            client_->Disconnect();
        }
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
     * @brief Отписка на инсрумент.
     */
    void Unsubscribe(std::string ticker) override {
        if (client_->Unsubscribe(ticker)) {
            tickers_.erase(std::move(ticker));
        }
    }

    /**
     * @brief Запрашивает snapshot у сервера.
     *
     * Залушечная реализация.
     *
     */
    void RequestSnapshot() override {}

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
     * Залушечная реализация.
     *
     * @return Текущее состояние клиентской библиотеки.
     */
    ConnectionState State() const noexcept override {};

private:
    /**
     * @brief Изменяет внутреннее состояние и уведомляет внешний код.
     *
     * @param new_state Новое состояние клиента.
     */
    void ChangeState(ConnectionState new_state) {};

    /**
     * @brief Сообщает об ошибке через пользовательский callback.
     *
     * Залушечная реализация.
     *
     * @param error Код ошибки.
     * @param message Текстовое описание ошибки.
     */
    void ReportError(ClientError error, std::string_view message) const {};

private:
    std::set<std::string> tickers_;
    CallbacksAdapter callbaсks_;
    tcp_client::ClientConfig config_;
    std::unique_ptr<tcp_client::TcpClient> client_;
};

std::unique_ptr<IOrderBookClient> MakesDefaultNetConfiguredClient(ClientCallbacks&& cc) {
    return std::make_unique<OrderBookClient>(std::move(cc));
}

std::unique_ptr<IOrderBookClient> MakeConfiguredClient(std::string host,
                                                       uint16_t port,
                                                       std::string name,
                                                       ClientCallbacks&& cc) {
    tcp_client::ClientConfig config{.host = std::move(host), .port = port, .client_name = name};

    return std::make_unique<OrderBookClient>(std::move(config), std::move(cc));
}

}  // namespace client_lib