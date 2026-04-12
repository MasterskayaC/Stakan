#pragma once

#include "command_queue.h"

#include <thread>
#include <atomic>

// Forward declarations — реализация в других модулях
class IClientList;
class OrderBook;
struct Snapshot;

/// @brief Broadcaster — отдельный поток, получающий команды на рассылку
/// через MPSC-очередь от потока OrderBook и сетевого потока.
///
/// Формирует snapshot в момент обработки команды.
class Broadcaster {
public:
    /// @brief Конструктор.
    /// @param clients Реестр подключённых TCP-клиентов (интерфейс IClientList).
    Broadcaster(IClientList& clients)
        : clients_(clients) {
    }

    /// @brief Деструктор — останавливает поток при уничтожении.
    ~Broadcaster() {
        stop();
    }

    /// @brief Добавляет команду в очередь. Вызывается из других потоков.
    void enqueue(std::unique_ptr<BroadcastCommand> cmd) {
        //заглушка
    }

    /// @brief Запускает поток-обработчик.
    void start() {
        // заглушка
        // running_ = true;
        // thread_ = std::thread(&Broadcaster::run, this);
        return;
    }

    /// @brief Останавливает поток-обработчик.
    void stop() {
        // заглушка
        // running_ = false;
        // разбудить поток (push специальной команды или notify)
        // if (thread_.joinable()) thread_.join();
        return;
    }

private:
    /// @brief Главный цикл обработки команд. Крутится в потоке.
    void run() {
        // заглушка
        // while (running_) {
        //     auto cmd = queue_.pop();
        //     switch (cmd.type) { ... }
        // }
        return;
    }

    /// @brief Отправляет snapshot конкретному клиенту.
    /// @param id Идентификатор сессии клиента.
    void handle_send_snapshot_to(SessionId id) {
        // заглушка
        return;
    }

    /// @brief Отправляет snapshot всем подключённым клиентам.
    void handle_send_snapshot_all() {
        // заглушка
        return;
    }

    /// @brief Отправляет MD Update всем подключённым клиентам.
    void handle_send_md_update() {
        // заглушка
        return;
    }


private:
    CommandQueue        queue_;           ///< MPSC-очередь команд.
    IClientList&        clients_;         ///< Интерфейс реестра клиентов.
    std::thread         thread_;          ///< Рабочий поток broadcaster'а.
    std::atomic<bool>   running_{false};  ///< Флаг работы потока.
};