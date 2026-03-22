#pragma once

#include "client_lib_interface.hpp"

#include <memory>

namespace client_lib {

/**
 * @brief Базовая заглушечная реализация клиентской библиотеки стакана.
 *
 * Класс реализует интерфейс @ref IOrderBookClient и служит промежуточным
 * адаптером между внешним кодом (CLI / GUI) и TCP-клиентом коллеги.
 *
 * На текущем этапе здесь находится только каркас реализации:
 * - хранение конфигурации и callback-ов;
 * - переключение внутренних состояний;
 * - точки интеграции с @c tcp_client::TcpClient;
 * - заглушки вместо полноценной сетевой логики.
 *
 * Полноценная обработка сообщений, конвертация транспортных структур
 * в @ref Snapshot и @ref TopLevel, а также потоковая работа будут
 * добавлены позднее.
 */
class OrderBookClient final : public IOrderBookClient {
public:
    /**
     * @brief Создаёт объект клиентской библиотеки.
     */
    OrderBookClient();

    /**
     * @brief Освобождает внутренние ресурсы.
     */
    ~OrderBookClient() override;

    OrderBookClient(const OrderBookClient&) = delete;
    OrderBookClient& operator=(const OrderBookClient&) = delete;
    OrderBookClient(OrderBookClient&&) noexcept;
    OrderBookClient& operator=(OrderBookClient&&) noexcept;

    /**
     * @brief Сохраняет набор пользовательских callback-функций.
     *
     * @param callbacks Обработчики событий клиента.
     */
    void SetCallbacks(ClientCallbacks callbacks) override;

    /**
     * @brief Сохраняет параметры подключения.
     *
     * На данном этапе метод не открывает соединение сам по себе,
     * а только подготавливает конфигурацию для последующего запуска.
     *
     * @param config Параметры подключения к серверу.
     */
    void Connect(const ClientConfig& config) override;

    /**
     * @brief Разрывает текущее соединение.
     *
     * В заглушечной реализации метод переводит клиент в состояние
     * @ref ConnectionState::Disconnected и вызывает транспортный
     * метод отключения, если транспорт уже был создан.
     */
    void Disconnect() override;

    /**
     * @brief Запускает клиентскую библиотеку.
     *
     * В дальнейшем здесь будет создаваться и запускаться транспортный
     * клиент, а также стартовать цикл приёма сообщений.
     * Сейчас метод выполняет только минимальную инициализацию.
     */
    void Start() override;

    /**
     * @brief Останавливает клиентскую библиотеку.
     *
     * В заглушечной реализации метод отключает транспорт и переводит
     * объект в состояние @ref ConnectionState::Stopped.
     */
    void Stop() override;

    /**
     * @brief Запрашивает snapshot у сервера.
     *
     * Обратите внимание: имя метода сохранено в точности таким же,
     * как в исходном интерфейсе, включая написание @c Smapshot.
     *
     * На текущем этапе здесь оставлена заглушка без реальной отправки
     * сетевого сообщения.
     */
    void RequestSmapshot() override;

    /**
     * @brief Проверяет наличие активного соединения.
     *
     * @return @c true, если внутренний транспорт считает себя подключённым.
     * @return @c false в противном случае.
     */
    bool IsConnected() const noexcept override;

    /**
     * @brief Возвращает текущее состояние клиента.
     *
     * @return Текущее состояние клиентской библиотеки.
     */
    ConnectionState State() const noexcept override;

private:
    /**
     * @brief Скрытая реализация объекта.
     *
     * Использование PIMPL позволяет не раскрывать транспортные детали
     * и зависимости от TCP-клиента в публичном заголовочном файле.
     */
    struct Impl;

    /**
     * @brief Изменяет внутреннее состояние и уведомляет внешний код.
     *
     * @param new_state Новое состояние клиента.
     */
    void ChangeState(ConnectionState new_state);

    /**
     * @brief Сообщает об ошибке через пользовательский callback.
     *
     * @param error Код ошибки.
     * @param message Текстовое описание ошибки.
     */
    void ReportError(ClientError error, std::string_view message) const;

private:
    std::unique_ptr<Impl> impl_;
};

} // namespace client_lib
