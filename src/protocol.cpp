#include "tcp_client/protocol.hpp"

namespace tcp_client {

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

std::array<std::uint8_t, 16> SerializeBestLevel(const BestLevelMessage& msg) {
    (void)msg;
    // Здесь будет упаковка best bid/ask в бинарный формат для передачи по сети.
    return {};
}

BestLevelMessage DeserializeBestLevel(const std::array<std::uint8_t, 16>& data) {
    (void)data;
    // Здесь будет распаковка входящего бинарного обновления best bid/ask.
    return {};
}

}  // namespace tcp_client
