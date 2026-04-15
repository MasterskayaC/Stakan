#pragma once

#include <atomic>
#include <iostream>
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
    /// @param io_ Ссылка на io_context для распараллеливания задач
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
        if (running_)
            return;
        running_ = true;
        thread_ = std::thread(&Broadcaster::run, this);
    }

    /// @brief Останавливает поток-обработчик.
    void stop() {
        if (!running_)
            return;
        running_ = false;
        if (thread_.joinable())
            thread_.join();
        return;
    }

private:
    /// @brief Главный цикл обработки команд. Крутится в потоке.
    void run() {
        while (running_) {
            auto cmd = queue_.pop();
            switch (cmd->type) {
                case CommandType::SendSnapshot:
                    if (cmd->client_id) {
                        boost::asio::post(io_, [this, cmd = std::move(cmd)] {
                            handle_send_snapshot_to(cmd->client_id.value(), cmd->get_data<common::Snapshot>()->get());
                        });
                    } else {
                        boost::asio::post(io_, [this, cmd = std::move(cmd)] {
                            handle_send_snapshot_all(cmd->get_data<common::Snapshot>()->get());
                        });
                    }
                    break;
                case CommandType::SendMDUpdate:
                    boost::asio::post(io_, [this, cmd = std::move(cmd)] {
                        handle_send_md_update();
                    });
                    break;
                default:
                    std::cout << "Error: either pass a snapshot or MDUpdate!\n";
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
    IClientList& clients_;              ///< Интерфейс реестра клиентов.
    boost::asio::io_context& io_;       ///< ссылка на io_context для распараллеливания
    CommandQueue queue_;                ///< MPSC-очередь команд.
    std::thread thread_;                ///< Рабочий поток broadcaster'а.
    std::atomic<bool> running_{false};  ///< Флаг работы потока.
};
