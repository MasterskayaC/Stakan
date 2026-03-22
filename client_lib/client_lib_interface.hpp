#pragma once

#include <cstdint>
#include <chrono>
#include <functional>
#include <string>
#include <string_view>

namespace client_lib {

    // Предварительное объявление структуры одного верхнего уровня стакана.
    // Полное определение можно будет подключить позже в отдельном файле.
    struct TopLevel;

    // Предварительное объявление структуры snapshot.
    // Snapshot — это снимок актуального состояния рынка на момент запроса/подключения.
    struct Snapshot;

    // Состояние клиентской библиотеки с точки зрения подключения и жизненного цикла.
    enum class ConnectionState {
        Stopped,        // Клиент остановлен и не выполняет никакой работы.
        Connecting,     // Идёт попытка установить соединение с сервером.
        Connected,      // Соединение успешно установлено.
        Reconnecting,   // Выполняется повторное подключение после разрыва.
        Disconnected    // Соединение отсутствует.
    };

    // Возможные типы ошибок, о которых библиотека может сообщить наружу.
    enum class ClientError {
        None,                   // Ошибки нет.
        SocketOpenFailed,       // Не удалось открыть сокет.
        ConnectFailed,          // Не удалось подключиться к серверу.
        ReadFailed,             // Ошибка чтения данных из сокета.
        WriteFailed,            // Ошибка отправки данных в сокет.
        ProtocolError,          // Получены некорректные данные или нарушен формат протокола.
        UnexpectedMessage,      // Получен неожиданный тип сообщения.
        SnapshotRequestFailed   // Не удалось запросить snapshot.
    };

    // Конфигурация подключения клиентской библиотеки.
    struct ClientConfig {
        // Адрес сервера, к которому нужно подключиться.
        std::string host{ "127.0.0.1" };

        // TCP-порт сервера.
        std::uint16_t port{ 0 };

        // Нужно ли автоматически пытаться переподключаться при обрыве соединения.
        bool auto_reconnect{ true };

        // Задержка между попытками переподключения.
        std::chrono::milliseconds reconnect_delay{ 1000 };
    };

    // Набор callback-функций, которые пользователь библиотеки может передать,
    // чтобы получать уведомления о событиях.
    struct ClientCallbacks {
        // Вызывается после успешного подключения к серверу.
        std::function<void()> on_connected;

        // Вызывается при разрыве соединения.
        std::function<void()> on_disconnected;

        // Вызывается при смене состояния клиента.
        std::function<void(ConnectionState)> on_state_changed;

        // Вызывается при получении snapshot.
        std::function<void(const Snapshot&)> on_snapshot;

        // Вызывается при получении обновления top-of-book.
        std::function<void(const TopLevel&)> on_top_of_book;

        // Вызывается при ошибке.
        // Передаёт тип ошибки и текстовое пояснение.
        std::function<void(ClientError, std::string_view)> on_error;
    };

    // Абстрактный интерфейс клиентской библиотеки.
    // Нужен для того, чтобы console/gui работали с единым API,
    // не завися от конкретной реализации.
    class IOrderBookClient {
    public:
        // Виртуальный деструктор нужен для корректного удаления объекта
        // через указатель на интерфейс.
        virtual ~IOrderBookClient() = default;

        // Устанавливает callback-функции, которые будут вызываться библиотекой
        // при подключении, ошибках, получении snapshot и обновлений.
        virtual void SetCallbacks(ClientCallbacks callbacks) = 0;

        // Настраивает подключение к серверу по заданной конфигурации.
        // Обычно здесь сохраняются host/port и параметры reconnect.
        virtual void Connect(const ClientConfig& config) = 0;

        // Разрывает соединение с сервером.
        virtual void Disconnect() = 0;

        // Запускает работу клиента.
        // Обычно здесь поднимается внутренний цикл обработки, сетевой поток и т.д.
        virtual void Start() = 0;

        // Полностью останавливает клиент и завершает его работу.
        virtual void Stop() = 0;

        // Запрашивает у сервера snapshot текущего состояния.
        virtual void RequestSnapshot() = 0;

        // Возвращает, активно ли сейчас соединение с сервером.
        virtual bool IsConnected() const noexcept = 0;

        // Возвращает текущее состояние клиента.
        virtual ConnectionState State() const noexcept = 0;
    };

} // namespace client_lib