#pragma once

#include <array>
#include <cstdint>

namespace tcp_client {

// Типы сообщений протокола.
enum class MessageType : std::uint8_t {
    NewOrder = 1,
    CancelOrder = 2,
    ReplaceOrder = 3,
    SnapshotRequest = 10,
    SnapshotResponse = 11,
    MarketDataUpdate = 12,
    Subscribe = 20,
    Unsubscribe = 21
};

// Сообщение торгового события.
struct OrderEventMessage {
    MessageType type{MessageType::NewOrder};
    std::uint32_t price{0};
    std::uint32_t qty{0};
    std::uint64_t order_id{0};
};

// Сообщение лучшего bid/ask для клиентов.
struct BestLevelMessage {
    std::uint32_t bid_price{0};
    std::uint32_t bid_qty{0};
    std::uint32_t ask_price{0};
    std::uint32_t ask_qty{0};
};

// Сериализация OrderEventMessage в 17 байт.
std::array<std::uint8_t, 17> SerializeOrderEvent(const OrderEventMessage& msg);

// Десериализация OrderEventMessage из 17 байт.
OrderEventMessage DeserializeOrderEvent(const std::array<std::uint8_t, 17>& data);

// Сериализация BestLevelMessage в 16 байт.
std::array<std::uint8_t, 16> SerializeBestLevel(const BestLevelMessage& msg);

// Десериализация BestLevelMessage из 16 байт.
BestLevelMessage DeserializeBestLevel(const std::array<std::uint8_t, 16>& data);

}  // namespace tcp_client
