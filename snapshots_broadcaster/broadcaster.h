#pragma once

#include "command_queue.h"

#include <thread>
#include <atomic>

// Forward declarations — реализация в других модулях у других людей
class IClientList;
class OrderBook;
struct Snapshot;

// Broadcaster отдельный поток, получающий команды на рассылку
// через MPSC-очередь от потока OrderBook и сетевого потока.
// Формирует snapshot в момент обработки команды 
class Broadcaster {
public:
    // order_book ссылка на order book для чтения текущего состояния стакана
    // clients реестр подключённых TCP-клиентов (IClientList) - интерфейс client list'a
    Broadcaster(const OrderBook& order_book, IClientList& clients)
        : order_book_(order_book)
        , clients_(clients) {
    }

    // Деструктор — останавливает поток при уничтожении
    ~Broadcaster() {
        stop();
    }

    // Добавляет команду в очередь. Вызывается из других потоков
    void enqueue(BroadcastCommand cmd) {
        //заглушка
    }

    // Запускает поток-обработчик
    void start() {
        // заглушка
        // running_ = true;
        // thread_ = std::thread(&Broadcaster::run, this);
        return;
    }

    // Останавливает поток-обработчик
    void stop() {
        // заглушка
        // running_ = false;
        // разбудить поток (push специальной команды или notify)
        // if (thread_.joinable()) thread_.join();
        return;
    }

private:
    // Главный цикл обработки команд. Крутится в потоке
    void run() {
        // заглушечка
        // while (running_) {
        //     auto cmd = queue_.pop();
        //     switch (cmd.type) { че то тама }
        // }
        return;
    }

    // Отправляет snapshot конкретному клиенту
    void handle_send_snapshot_to(SessionId id) {
        // заглушка
        return;
    }

    // Отправляет snapshot всем подключённым клиентам
    void handle_send_snapshot_all() {
        // заглушка
        return;
    }

    // Отправляет MD Update всем подключённым клиентам.
    void handle_send_md_update() {
        // заглушка
        return;
    }


private:
    CommandQueue        queue_;
    const OrderBook&    order_book_;
    IClientList&        clients_;       // интерфейс client list'a
    std::thread         thread_;
    std::atomic<bool>   running_{false};
};
