#pragma once

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <optional>
#include <variant>

#include "bid_ask_interface.h"

using SessionId = std::size_t;

struct MDUpdate {  // TMP for std::variant
    enum class UpdateType : uint8_t { Add, Modify, Delete };

    enum class Side : uint8_t { Bid, Ask };

    UpdateType type;
    Side side;
    common::Order order;
};

/// @brief Типы команд — что broadcaster должен делать.
enum class CommandType : uint8_t {
    SendSnapshot,  ///< Отправить snapshot
    SendMDUpdate,  ///< Отправить MD Update
};

/// @brief Команда для broadcaster'а — тип + кому слать (если кому-то конкретному).
struct BroadcastCommand {
    CommandType type;
    std::optional<SessionId> client_id;             ///< конкретному клиенту или всем подписанным.
    std::variant<common::Snapshot, MDUpdate> data;  // либо шаблон вместо std::variant
};

/// @brief MPSC очередь команд с логикой поглощения.
///
/// SendSnapshotAll поглощает все ожидающие SendSnapshotTo, поставленные до него.
/// SendMDUpdate не участвует в поглощении.
class CommandQueue {
public:
    /// @brief Добавляет команду в очередь.
    ///
    /// Вызывается из потоков OrderBook и сетевого потока.
    void push(BroadcastCommand cmd) {
        std::unique_lock<std::mutex> ulck(mutex_);
        if(cmd.client_id == std::nullopt)queue_.clear();
        queue_.push_back(cmd);
        cv_.notify_one();
    }

    /// @brief Извлекает следующую команду из очереди.
    ///
    /// Блокирует вызывающий поток, если очередь пуста.
    BroadcastCommand pop() {
        std::unique_lock<std::mutex> ulck(mutex_);
        cv_.wait(ulck,[this]{return !queue_.empty();});
        BroadcastCommand cmd = queue_.front();
        queue_.pop_front();
        return cmd;
    }

    /// @brief Неблокирующее извлечение.
    /// @return Команда или std::nullopt, если очередь пуста.
    std::optional<BroadcastCommand> try_pop() {
        // заглушка
        return std::nullopt;
    }

private:
    std::deque<BroadcastCommand> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};