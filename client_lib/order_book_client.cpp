#include "order_book_client.hpp"

#include <utility>

namespace client_lib {


 /**
 * @brief сюда будет прикрущен адаптер IClientCallbacks из транспортного TCP модуля наверное
 */
struct OrderBookClient::Impl {

};

/**
 * @brief Создаёт объект клиентской библиотеки.
 */
OrderBookClient::OrderBookClient() {
     
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

}

/**
 * @brief Сохраняет конфигурацию подключения.
 *
 *
 * @param config Параметры подключения к серверу.
 */
void OrderBookClient::Connect(const ClientConfig& config) {

}

/**
 * @brief Разрывает текущее логическое соединение.
 *
 */
void OrderBookClient::Disconnect() {
    
}

/**
 * @brief Запускает клиентскую библиотеку.
 *
 */
void OrderBookClient::Start() {
    
}

/**
 * @brief Останавливает клиентскую библиотеку.
 *
 */
void OrderBookClient::Stop() {
    
}

/**
 * @brief Запрашивает snapshot текущего состояния.
 *
 */
void OrderBookClient::RequestSmapshot() {   

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
    return false;
}

/**
 * @brief Возвращает текущее состояние клиента.
 *
 * @return Текущее состояние из внутреннего хранилища.
 */
ConnectionState OrderBookClient::State() const noexcept {
    return ConnectionState{};
}

/**
 * @brief Меняет внутреннее состояние клиента.
 *
 *
 * @param new_state Новое состояние клиента.
 */
void OrderBookClient::ChangeState(ConnectionState new_state) {
    
}

/**
 * @brief Сообщает об ошибке через пользовательский callback.
 *
 * @param error Код ошибки.
 * @param message Текстовое описание ошибки.
 */
void OrderBookClient::ReportError(ClientError error, std::string_view message) const {
    
}

} // namespace client_lib
