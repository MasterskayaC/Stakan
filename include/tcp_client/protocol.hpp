#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

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
    // Сообщение о подписке на инструмент
    struct SubscriptionMessage {
        MessageType type{MessageType::Subscribe};
        std::string symbol;
    };

    // Сообщение торгового события.
    struct OrderEventMessage {
        MessageType type{MessageType::NewOrder};
        std::uint32_t price{0};
        std::uint32_t qty{0};
        std::uint64_t order_id{0};
    };

    std::vector<std::uint8_t> SerializeSubscription(const SubscriptionMessage& msg);
    // @ToDo - т.к. мы в будущем планируем работу с ордерами
    // Сериализация OrderEventMessage в 17 байт.
    std::array<std::uint8_t, 17> SerializeOrderEvent(const OrderEventMessage& msg);

    // Десериализация OrderEventMessage из 17 байт.
    OrderEventMessage DeserializeOrderEvent(const std::array<std::uint8_t, 17>& data);

}  // namespace tcp_client
