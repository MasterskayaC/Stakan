#pragma once

#include <cstdint>
#include <vector>

// Один уровень цены в стакане — цена и количество, ничего лишнего
struct PriceLevel {
    uint32_t price;
    uint32_t quantity;
};

// Снэпшот стакана — бидики и аскики, весь расклад на текущий момент
struct Snapshot {
    std::vector<PriceLevel> bids;
    std::vector<PriceLevel> asks;

    // Сериализует snapshot в буфер для отправки по сети
    // ну и возвращает вектор байт, готовый к передаче через async_send.
    std::vector<char> serialize() const {
        // заглушка
        return {};
    }
};
