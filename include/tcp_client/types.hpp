#pragma once

#include <cstdint>
#include <string>

namespace tcp_client {

enum class Side : std::uint8_t {
    Bid = 0,
    Ask = 1
};

struct PriceLevel {
    double price{0.0};
    double quantity{0.0};
};

struct TopOfBook {
    std::string symbol;
    PriceLevel best_bid{};
    PriceLevel best_ask{};
    std::uint64_t exchange_timestamp_ns{0};
};

}  // namespace tcp_client
