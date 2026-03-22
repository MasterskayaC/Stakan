#pragma once

#include <cstdint>
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace client_lib {

    /**
     * @brief Структура верхнего уровня стакана.
     *
     * Полное определение пока в другом коде
     */
    struct TopLevel;

    /**
     * @brief Структура snapshot-снимка состояния рынка.
     *
     * Полное определение пока в другом коде
     */
    struct Snapshot;

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
     * @brief Конфигурация подключения клиента.
     */
    struct ClientConfig {
        /// Адрес сервера.
        std::string host{ "127.0.0.1" };

        /// TCP-порт сервера.
        std::uint16_t port{ 0 };

        /// Флаг автоматического переподключения при обрыве связи.
        bool auto_reconnect{ true };

        /// Задержка между попытками переподключения.
        std::chrono::milliseconds reconnect_delay{ 1000 };
    };

    /**
     * @brief Набор callback-функций для уведомления внешнего кода о событиях клиента.
     */
    struct ClientCallbacks {
        /// Вызывается после успешного подключения к серверу.
        std::function<void()> on_connected;

        /// Вызывается при разрыве соединения.
        std::function<void()> on_disconnected;

        /// Вызывается при изменении состояния клиента.
        std::function<void(ConnectionState)> on_state_changed;

        /// Вызывается при получении snapshot.
        std::function<void(const Snapshot&)> on_snapshot;

        /// Вызывается при получении обновления top-of-book.
        std::function<void(const TopLevel&)> on_top_of_book;

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
         * @brief Устанавливает callback-функции клиента.
         *
         * @param callbacks Набор обработчиков событий.
         */
        virtual void SetCallbacks(ClientCallbacks callbacks) = 0;

        /**
         * @brief Настраивает подключение по переданной конфигурации.
         *
         * @param config Параметры подключения к серверу.
         */
        virtual void Connect(const ClientConfig& config) = 0;

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
        virtual void Start() = 0;

        /**
         * @brief Останавливает работу клиента.
         */
        virtual void Stop() = 0;

        /**
         * @brief Запрашивает snapshot текущего состояния у сервера.
         */
        virtual void RequestSmapshot() = 0;

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

} // namespace client_lib
