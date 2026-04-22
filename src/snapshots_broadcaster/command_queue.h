#pragma once

#include <condition_variable>
#include <deque>
#include <functional>  // для std::reference_wrapper
#include <mutex>
#include <optional>
#include <typeinfo>

#include "../bid_ask_snaphot_interface/bid_ask_interface.h"

using SessionId = std::size_t;

struct MDUpdate {  // TMP for std::variant
    enum class UpdateType : uint8_t { Add, Modify, Delete };

    UpdateType type;
    bool is_bid = true;
    common::Order order;
};

/// @brief Типы команд — что broadcaster должен делать.
enum class CommandType : uint8_t {
    SendSnapshot,  ///< Отправить snapshot
    SendMDUpdate,  ///< Отправить MD Update
};

/// @brief Команда для broadcaster'а — тип + кому слать (если кому-то
/// конкретному).
struct BroadcastCommand {
    CommandType type;
    std::optional<SessionId> client_id;

    BroadcastCommand(CommandType t, std::optional<SessionId> id = std::nullopt)
                            : type(t), client_id(id) 
    {}

    virtual ~BroadcastCommand() = default;

    // Возвращает ссылку на данные, если тип совпадает, иначе nullopt
    template<typename T>
    std::optional<std::reference_wrapper<const T>> get_data() const {
        if (get_data_type() == typeid(T)) {
            return std::cref(*static_cast<const T*>(get_data_ptr()));
        }
        return std::nullopt;
    }

protected:
    virtual const std::type_info& get_data_type() const = 0;
    virtual const void* get_data_ptr() const = 0;
};

struct BroadcastSnapshotCommand : BroadcastCommand {
public:
    common::Snapshot data;

    explicit BroadcastSnapshotCommand(common::Snapshot snapshot)
                        : BroadcastCommand(CommandType::SendSnapshot)
                        , data(std::move(snapshot)) 
    {}

    BroadcastSnapshotCommand(std::optional<SessionId> id, common::Snapshot snapshot)
                        : BroadcastCommand(CommandType::SendSnapshot, id)
                        , data(std::move(snapshot))
    {}

    const common::Snapshot& get_snapshot() const { return data; }

protected:
    const std::type_info& get_data_type() const override {
        return typeid(common::Snapshot);
    }

    const void* get_data_ptr() const override {
        return &data;
    }
};

struct BroadcastMDUpdateCommand : BroadcastCommand {
public:
    MDUpdate data;

    explicit BroadcastMDUpdateCommand(MDUpdate update)
                        : BroadcastCommand(CommandType::SendMDUpdate)
                        , data(std::move(update))
    {}

    BroadcastMDUpdateCommand(std::optional<SessionId> id, MDUpdate update)
                        : BroadcastCommand(CommandType::SendMDUpdate, id)
                        , data(std::move(update))
    {}

    const MDUpdate& get_update() const { return data; }

protected:
    const std::type_info& get_data_type() const override {
        return typeid(MDUpdate);
    }

    const void* get_data_ptr() const override {
        return &data;
    }
};

/// @brief MPSC очередь команд с логикой поглощения.
///
/// SendSnapshotAll поглощает все ожидающие SendSnapshotTo, поставленные до
/// него. SendMDUpdate не участвует в поглощении.
class CommandQueue {
public:
    /// @brief Добавляет команду в очередь.
    ///
    /// Вызывается из потоков OrderBook и сетевого потока.
    void push(std::unique_ptr<BroadcastCommand> cmd) {
        std::unique_lock<std::mutex> ulck(mutex_);
        if (cmd->client_id == std::nullopt)
            queue_.clear();
        queue_.push_back(std::move(cmd));
        cv_.notify_one();
    }

    /// @brief Извлекает следующую команду из очереди.
    ///
    /// Блокирует вызывающий поток, если очередь пуста.
    std::unique_ptr<BroadcastCommand> pop() {
        std::unique_lock<std::mutex> ulck(mutex_);
        cv_.wait(ulck, [this] {
            return !queue_.empty();
        });
        std::unique_ptr<BroadcastCommand> cmd = std::move(queue_.front());
        queue_.pop_front();
        return cmd;
    }

    /// @brief Неблокирующее извлечение.
    /// @return Команда или std::nullopt, если очередь пуста.
    std::unique_ptr<BroadcastCommand> try_pop() {
        // заглушка
        return nullptr;
    }

private:
    std::deque<std::unique_ptr<BroadcastCommand>>  queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
