#pragma once

#include <cstdint>
#include <optional>
#include <deque>
#include <mutex>
#include <condition_variable>

using SessionId = std::size_t;

/// @brief Типы команд — что broadcaster должен делать.
enum class CommandType : uint8_t {
    SendSnapshotTo,   ///< Отправить snapshot конкретному клиенту.
    SendSnapshotAll,  ///< Отправить snapshot всем клиентам.
    SendMDUpdate,     ///< Отправить MD Update всем.
};

/// @brief Команда для broadcaster'а — тип + кому слать (если кому-то конкретному).
struct BroadcastCommand {
    CommandType type;
    SessionId   client_id;  ///< Используется только для SendSnapshotTo.
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
        // заглушка
        // 1. lock mutex_
        // 2. если cmd.type == SendSnapshotAll — удалить все SendSnapshotTo из очереди
        // 3. добавить cmd в очереди
        // 4. notify_one на cv_
        return;
    }

    /// @brief Извлекает следующую команду из очереди.
    ///
    /// Блокирует вызывающий поток, если очередь пуста.
    BroadcastCommand pop() {
        // заглушка
        // - lock mutex_
        // - cv_.wait пока очередь пуста
        // - забрать front, pop_front
        // - возвращаем команду
        return BroadcastCommand{};
    }

    /// @brief Неблокирующее извлечение.
    /// @return Команда или std::nullopt, если очередь пуста.
    std::optional<BroadcastCommand> try_pop() {
        // заглушка
        return std::nullopt;
    }

private:
    std::deque<BroadcastCommand>  queue_;
    std::mutex                    mutex_;
    std::condition_variable       cv_;
};
