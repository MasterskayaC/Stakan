#pragma once

#include <cstdint>
#include <optional>
#include <deque>
#include <mutex>
#include <condition_variable>

using SessionId = std::size_t;

enum class CommandType : uint8_t {
    SendSnapshotTo,   // отправить snapshot нашему конкретному клиенту
    SendSnapshotAll,  // отправить snapshot всем клиентикам
    SendMDUpdate,     // отправить MD Update всем
};

struct BroadcastCommand {
    CommandType type;
    SessionId   client_id;  // только для SendSnapshotTo
};

// MPSC очередь команд с логикой поглощения
// SendSnapshotAll поглощает все ожидающие SendSnapshotTo, поставленные до него
// SendMDUpdate не участвует в поглощении
class CommandQueue {
public:
    // Добавляет команду в очередь
    // вызывается из потоков OrderBook и сетевого потока
    void push(BroadcastCommand cmd) {
        // заглушка
        // 1. lock mutex_
        // 2. если cmd.type == SendSnapshotAll — удалить все SendSnapshotTo из очереди
        // 3. добавить cmd в очереди
        // 4. notify_one на cv_
        return;
    }

    // Извлекает следующую команду из очереди
    //  и я ее блокирую, если очередь пуста
    BroadcastCommand pop() {
        // заглушка
        // - lock mutex_
        // - cv_.wait пока очередь пуста
        // - забрать front, pop_front
        // - возвращаем команду
        return BroadcastCommand{};
    }

    // Неблокирующее извлечение. Возвращает nullopt, если очередь пуста
    std::optional<BroadcastCommand> try_pop() {
        // заглушка
        return std::nullopt;
    }

private:
    std::deque<BroadcastCommand>  queue_;
    std::mutex                    mutex_;
    std::condition_variable       cv_;
};
