#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <tcp_client/client.hpp>

namespace client_lib {

/**
 * @brief Состояние клиентской библиотеки.
 */
enum class ConnectionState {
    Error,        ///< Клиент в ошибке.
    Connected,    ///< Соединение установлено.
    Disconnected  ///< Соединение неустановлено.
};

/**
 * @brief Набор callback-функций для уведомления внешнего кода о событиях клиента.
 */
struct ClientCallbacks final {
    /// Вызывается после успешного подключения к серверу.
    std::function<void()> on_connected;

    /// Вызывается при разрыве соединения.
    std::function<void()> on_disconnected;

    /// Вызывается при получении обновления Snapshot.
    std::function<void(const common::Snapshot&)> on_snapshot;

    /**
     * @brief Вызывается при возникновении ошибки.
     *
     * @param error   - Переводит клиента в состояние ошибки.
     * @param message - сообщение ошибки.
     */
    std::function<void(ConnectionState, std::string_view)> on_error;
};

/**
 * @brief Абстрактный интерфейс клиентской библиотеки.
 *
 * Позволяет использовать единый API для console/gui независимо
 * от конкретной реализации клиента.
 */
class IOrderBookClient {
public:
    /**
     * @brief Виртуальный деструктор интерфейса.
     */
    virtual ~IOrderBookClient() = default;

    /**
     * @brief Открывает сооединение с сервером.
     */
    virtual void Connect() = 0;

    /**
     * @brief Разрывает соединение с сервером.
     */
    virtual void Disconnect() = 0;

    /**
     * @brief Подписка на обновления по инструменту.
     *
     * @param ticket - Инструмент
     */
    virtual void Subscribe(std::string ticker) = 0;

    /**
     * @brief Отписка от обновлений по инструменту.
     *
     * @param ticket - Инструмент
     */
    virtual void Unsubscribe(std::string ticker) = 0;

    /**
     * @brief Проверяет наличие активного соединения.
     *
     * @return true, если соединение активно.
     * @return false, если соединения нет.
     */
    virtual bool IsConnected() const noexcept = 0;

    /**
     * @brief Возвращает текущее состояние клиента.
     *
     * @return Текущее значение состояния ConnectionState.
     */
    virtual ConnectionState State() const noexcept = 0;
};

/** @brief Создает объект client lib с дефолной конфиграцией соединения.
 *
 * @param cc - ClientCallbacks структура callback-ов от ui.
 */
std::unique_ptr<IOrderBookClient> MakesDefaultNetConfiguredClient(ClientCallbacks&& cc);

/** @brief Создает объект client lib с параметрами соединения из UI.
 *
 * @param host - IP адрес сервера.
 * @param port - Порт.
 * @param cc   - ClientCallbacks структура callback-ов от ui.
 */
std::unique_ptr<IOrderBookClient> MakeConfiguredClient(std::string host,
                                                       uint16_t port,
                                                       std::string name,
                                                       ClientCallbacks&& cc);

}  // namespace client_lib
