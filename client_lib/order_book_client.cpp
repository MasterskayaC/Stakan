#include "order_book_client.hpp"

#include <utility>

namespace client_lib {

/**
 * @brief Скрытая реализация класса @ref OrderBookClient.
 *
 * На текущем этапе структура хранит только внутреннее состояние
 * клиентской библиотеки и не зависит от транспортного кода.
 */
struct OrderBookClient::Impl {
    /**
     * @brief Последняя сохранённая конфигурация подключения.
     */
    ClientConfig config{};

    /**
     * @brief Пользовательские callback-функции.
     */
    ClientCallbacks callbacks{};

    /**
     * @brief Текущее состояние клиента.
     */
    ConnectionState state{ConnectionState::Stopped};

    /**
     * @brief Признак того, что клиент был запущен методом @ref OrderBookClient::Start.
     */
    bool started{false};

    /**
     * @brief Признак логического подключения.
     *
     * В заглушечной реализации используется вместо реального сокета.
     */
    bool connected{false};
};

/**
 * @brief Создаёт объект клиентской библиотеки.
 */
OrderBookClient::OrderBookClient()
    : impl_(std::make_unique<Impl>()) {
}

/**
 * @brief Освобождает ресурсы объекта.
 */
OrderBookClient::~OrderBookClient() = default;

/**
 * @brief Перемещает объект клиентской библиотеки.
 */
OrderBookClient::OrderBookClient(OrderBookClient&&) noexcept = default;

/**
 * @brief Перемещающее присваивание для объекта клиентской библиотеки.
 *
 * @return Ссылка на текущий объект.
 */
OrderBookClient& OrderBookClient::operator=(OrderBookClient&&) noexcept = default;

/**
 * @brief Сохраняет пользовательские callback-функции.
 *
 * @param callbacks Набор обработчиков событий.
 */
void OrderBookClient::SetCallbacks(ClientCallbacks callbacks) {
    impl_->callbacks = std::move(callbacks);
}

/**
 * @brief Сохраняет конфигурацию подключения.
 *
 * В текущей заглушечной реализации реальное сетевое соединение
 * не открывается. Метод только сохраняет параметры и переводит
 * клиента в состояние подготовки к подключению.
 *
 * @param config Параметры подключения к серверу.
 */
void OrderBookClient::Connect(const ClientConfig& config) {
    impl_->config = config;
    impl_->connected = false;
    ChangeState(ConnectionState::Connecting);
}

/**
 * @brief Разрывает текущее логическое соединение.
 *
 * В заглушечной реализации метод только сбрасывает внутренний флаг
 * подключения, переводит состояние в @ref ConnectionState::Disconnected
 * и уведомляет внешний код через callback.
 */
void OrderBookClient::Disconnect() {
    const bool was_connected = impl_->connected;

    impl_->connected = false;
    ChangeState(ConnectionState::Disconnected);

    if (was_connected && impl_->callbacks.on_disconnected) {
        impl_->callbacks.on_disconnected();
    }
}

/**
 * @brief Запускает клиентскую библиотеку.
 *
 * На текущем этапе метод не создаёт транспортный клиент и не запускает
 * сетевую обработку. Вместо этого он переводит объект в состояние
 * @ref ConnectionState::Connected и вызывает callback успешного запуска.
 */
void OrderBookClient::Start() {
    impl_->started = true;
    impl_->connected = true;

    ChangeState(ConnectionState::Connected);

    if (impl_->callbacks.on_connected) {
        impl_->callbacks.on_connected();
    }
}

/**
 * @brief Останавливает клиентскую библиотеку.
 *
 * Метод очищает внутренние флаги состояния и переводит объект
 * в состояние @ref ConnectionState::Stopped.
 */
void OrderBookClient::Stop() {
    const bool was_connected = impl_->connected;

    impl_->started = false;
    impl_->connected = false;

    if (was_connected && impl_->callbacks.on_disconnected) {
        impl_->callbacks.on_disconnected();
    }

    ChangeState(ConnectionState::Stopped);
}

/**
 * @brief Запрашивает snapshot текущего состояния.
 *
 * На данном этапе метод является заглушкой и не выполняет реальный
 * сетевой запрос. Если клиент не находится в состоянии подключения,
 * вызывается callback ошибки.
 */
void OrderBookClient::RequestSmapshot() {
    if (!IsConnected()) {
        ReportError(
            ClientError::SnapshotRequestFailed,
            "Snapshot request is unavailable without active connection"
        );
        return;
    }

    /**
     * @todo После интеграции с транспортным слоем здесь должен появиться
     * код отправки запроса snapshot и обработки ответа от сервера.
     */
}

/**
 * @brief Проверяет, считает ли объект себя подключённым.
 *
 * @return @c true, если клиент находится в логическом подключении.
 * @return @c false в противном случае.
 */
bool OrderBookClient::IsConnected() const noexcept {
    return impl_->connected;
}

/**
 * @brief Возвращает текущее состояние клиента.
 *
 * @return Текущее состояние из внутреннего хранилища.
 */
ConnectionState OrderBookClient::State() const noexcept {
    return impl_->state;
}

/**
 * @brief Меняет внутреннее состояние клиента.
 *
 * После изменения состояния метод уведомляет внешний код через
 * callback @c on_state_changed, если он был установлен.
 *
 * @param new_state Новое состояние клиента.
 */
void OrderBookClient::ChangeState(ConnectionState new_state) {
    impl_->state = new_state;

    if (impl_->callbacks.on_state_changed) {
        impl_->callbacks.on_state_changed(new_state);
    }
}

/**
 * @brief Сообщает об ошибке через пользовательский callback.
 *
 * @param error Код ошибки.
 * @param message Текстовое описание ошибки.
 */
void OrderBookClient::ReportError(ClientError error, std::string_view message) const {
    if (impl_->callbacks.on_error) {
        impl_->callbacks.on_error(error, message);
    }
}

} // namespace client_lib
