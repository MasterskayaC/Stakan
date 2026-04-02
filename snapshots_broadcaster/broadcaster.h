#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include "bid_ask_interface.h"
#include "client_list.h"
#include "command_queue.h"

// Forward declarations — реализация в других модулях

/// @brief Broadcaster — отдельный поток, получающий команды на рассылку
/// через MPSC-очередь от потока OrderBook и сетевого потока.
///
/// Формирует snapshot в момент обработки команды.
class Broadcaster {
public:
    /// @brief Конструктор.
    /// @param clients Реестр подключённых TCP-клиентов (интерфейс IClientList).
    Broadcaster(IClientList& clients, boost::asio::io_context& io) : clients_(clients), io_(io) {}

    /// @brief Деструктор — останавливает поток при уничтожении.
    ~Broadcaster() {
        stop();
    }

    /// @brief Добавляет команду в очередь. Вызывается из других потоков.
    void enqueue(std::unique_ptr<BroadcastCommand> cmd) {
        queue_.push(std::move(cmd));
    }

    /// @brief Запускает поток-обработчик.
    void start() {
        running_ = true;
        thread_ = std::thread(&Broadcaster::run, this);
    }

    /// @brief Останавливает поток-обработчик.
    void stop() {
        running_ = false;
        if (thread_.joinable()) thread_.join();
        return;
    }

private:
    /// @brief Главный цикл обработки команд. Крутится в потоке.
    void run() {
        while (running_) {
            auto cmd = queue_.pop();
            switch (cmd->type){
                case CommandType::SendSnapshot:
                    if(cmd->client_id == std::nullopt){
                        boost::asio::post(io_, [this, cmd = std::move(cmd)]{handle_send_snapshot_all(cmd->get_data<common::Snapshot>()->get());}); 
                    }
                    else{
                        boost::asio::post(io_, [this, cmd = std::move(cmd)]{handle_send_snapshot_to(cmd->client_id.value(), cmd->get_data<common::Snapshot>()->get());}); 
                    }
                    break;
                default:
                    boost::asio::post(io_, [this, cmd = std::move(cmd)]{handle_send_md_update();}); 
                    break;
                    
            }

        }
        return;
    }

    /// @brief Отправляет snapshot конкретному клиенту.
    /// @param id Идентификатор сессии клиента.
    void handle_send_snapshot_to(SessionId id, common::Snapshot s) {
        std::vector<char> bytes = s.Serialize();
        clients_.broadcast_to_certain(id, bytes);
    }

    /// @brief Отправляет snapshot всем подключённым клиентам.
    void handle_send_snapshot_all(common::Snapshot s) {
        std::vector<char> bytes = s.Serialize();
        clients_.broadcast_to_subscribed(bytes);
    }

    /// @brief Отправляет MD Update всем подключённым клиентам.
    void handle_send_md_update() {
        // заглушка
        return;
    }

private:
    CommandQueue queue_;                ///< MPSC-очередь команд.
    IClientList& clients_;              ///< Интерфейс реестра клиентов.
    std::thread thread_;                ///< Рабочий поток broadcaster'а.
    std::atomic<bool> running_{false};  ///< Флаг работы потока.
    boost::asio::io_context& io_;
};
