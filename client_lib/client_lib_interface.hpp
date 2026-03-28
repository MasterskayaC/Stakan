#pragma once

#include <cstdint>
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <memory>

namespace client_lib {

    /**
     * @brief Структура snapshot-снимка состояния рынка.
     *
     * Полное определение пока в другом коде
     */
    struct Snapshot { int price; };

    /**
     * @brief Состояние клиентской библиотеки.
     */
    enum class ConnectionState {
        Stopped,       ///< Клиент остановлен.
        Connecting,    ///< Выполняется подключение к серверу.
        Connected,     ///< Соединение с сервером установлено.
        Reconnecting,  ///< Выполняется повторное подключение после разрыва.
        Disconnected   ///< Соединение отсутствует.
    };

    /**
     * @brief Типы ошибок, которые может вернуть клиентская библиотека.
     */
    enum class ClientError {
        None,                   ///< Ошибки нет.
        SocketOpenFailed,       ///< Не удалось открыть сокет.
        ConnectFailed,          ///< Не удалось подключиться к серверу.
        ReadFailed,             ///< Ошибка чтения из сокета.
        WriteFailed,            ///< Ошибка записи в сокет.
        ProtocolError,          ///< Ошибка формата или обработки протокола.
        UnexpectedMessage,      ///< Получено неожиданное сообщение.
        SnapshotRequestFailed   ///< Не удалось запросить snapshot.
    };

    /**
     * @brief Набор callback-функций для уведомления внешнего кода о событиях клиента.
     */
    struct ClientCallbacks final {
        /// Вызывается после успешного подключения к серверу.
        std::function<void()> on_connected;

        /// Вызывается при разрыве соединения.
        std::function<void()> on_disconnected;

        /// Вызывается при получении обновления top-of-book.
        std::function<void(const Snapshot&)> on_snapshot;

        /**
         * @brief Вызывается при возникновении ошибки.
         *
         * @param error Код ошибки.
         * @param message Текстовое описание ошибки.
         */
        std::function<void(ClientError, std::string_view)> on_error;
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
         * @brief Запускает работу клиента.
         *
         * Обычно здесь стартует внутренняя логика обработки,
         * сетевой поток и приём сообщений.
         */
        virtual void Subscribe(std::string ticker) = 0;

        /**
         * @brief Останавливает работу клиента.
         */
        virtual void Unsubscribe(std::string ticker) = 0;

        /**
         * @brief Запрашивает snapshot текущего состояния у сервера.
         */
        virtual void RequestSnapshot() = 0;

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

    std::unique_ptr<IOrderBookClient> MakesDefaultNetConfiguredClient(ClientCallbacks&& cc);

} // namespace client_lib
