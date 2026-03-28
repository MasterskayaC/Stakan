#pragma once

#include "client_lib_interface.hpp"

/** @todo Разобраться с включением TCP-client */
#include "../include/tcp_client/client.hpp"

#include <memory>
#include <set>
#include <utility>

namespace client_lib {

/** @brief Создает объект client lib с дефолной конфиграцией соединения
 * 
 * @param сс ClientCallbacks структура callback-ов от ui;
 */
std::unique_ptr<IOrderBookClient> MakesDefaultNetConfiguredClient(ClientCallbacks&& cc) {
    return std::make_unique<IOrderBookClient>(std::move(cc));
}

 /**
 * @brief Реализация интерфеса IClientCallbacks
 */
class CallbacksAdapter : public tcp_client::IClientCallbacks {
public:
    CallbacksAdapter(ClientCallbacks&& cc)
        : cc_(std::move(cc)) {}

    ~CallbacksAdapter() = default;

    void OnConnected() override {
        cc_.on_connected;
    }

    void OnDisconnected() override {
        cc_.on_disconnected;
    }

    void OnTopOfBook(const tcp_client::TopOfBook& update) override {
        /** @todo сделать из TopOfBook какой-то Snapshot*/
        Snapshot snp;
        cc_.on_snapshot(snp);
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
     * @brief Создаёт объект клиентской библиотеки.
     */
    OrderBookClient::OrderBookClient(ClientCallbacks&& cc) 
        : callbaks_(std::move(cc)) {
        client_ = std::make_unique<tcp_client::TcpClient>(config_, callbaks_);
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
     *
     * В заглушечной реализации метод переводит клиент в состояние
     * @ref ConnectionState::Disconnected и вызывает транспортный
     * метод отключения, если транспорт уже был создан.
     */
    void Disconnect() override {
        if (IsConnected()) {
            client_->Disconnect();
        }
    }

    /**
     * @brief Подписка на инструмент.
     *
     * В дальнейшем здесь будет создаваться и запускаться транспортный
     * клиент, а также стартовать цикл приёма сообщений.
     * Сейчас метод выполняет только минимальную инициализацию.
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
     * @param error Код ошибки.
     * @param message Текстовое описание ошибки.
     */
    void ReportError(ClientError error, std::string_view message) const {};

private:
    std::set<std::string> tickers_;
    CallbacksAdapter callbaks_;
    tcp_client::ClientConfig config_;
    std::unique_ptr<tcp_client::TcpClient> client_;
};

} // namespace client_lib
