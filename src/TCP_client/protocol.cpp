#include "../../include/tcp_client/protocol.hpp"

namespace tcp_client {
    std::vector<std::uint8_t> SerializeSubscription(const SubscriptionMessage &msg) {
        std::vector<std::uint8_t> buffer;

        const auto len = static_cast<std::uint16_t>(msg.symbol.size());

        buffer.reserve(1 + 2 + len);

        // type
        buffer.push_back(static_cast<std::uint8_t>(msg.type));

        // Length
        buffer.push_back(static_cast<std::uint8_t>((len >> 8) & 0xFF));
        buffer.push_back(static_cast<std::uint8_t>(len & 0xFF));

        // payload
        buffer.insert(buffer.end(), msg.symbol.begin(), msg.symbol.end());

        return buffer;
    }

    //@ToDo - определить протокол обмена ордерами и реализовать далее
    std::array<std::uint8_t, 17> SerializeOrderEvent(const OrderEventMessage& msg) {
        (void)msg;
        // Здесь будет упаковка полей сообщения в байтовый массив в согласованном порядке байт.
        return {};
    }

    OrderEventMessage DeserializeOrderEvent(const std::array<std::uint8_t, 17>& data) {
        (void)data;
        // Здесь будет распаковка байтового массива в структуру сообщения в согласованном порядке байт.
        return {};
    }
} // namespace tcp_client