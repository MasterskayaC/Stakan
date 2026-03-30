// Минимальная цель сборки: без mongocxx, только C++17.
// Запуск: build\Release\compile_test.exe — печатает тестовые числа для отчёта/демо.

#include <cstdint>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "compile_test: сборка OK.\n\n";

    // Учебный бинарный кадр входящего ордера: [Type 1B][Price 4B][Qty 4B][OrderId 8B]
    constexpr std::uint8_t  message_type = 1;   // условно NEW ORDER
    constexpr std::int32_t  price          = 10050;
    constexpr std::int32_t  qty            = 100;
    constexpr std::uint64_t order_id       = 9001;

    std::cout << "Тестовый ордер:\n"
              << "  message_type = " << static_cast<int>(message_type) << '\n'
              << "  price        = " << price << '\n'
              << "  qty          = " << qty << '\n'
              << "  order_id     = " << order_id << "\n\n";

    // Рассылка лучшего уровня: [BID_PRICE][BID_QTY][ASK_PRICE][ASK_QTY]
    constexpr std::int32_t bid_price = 10050;
    constexpr std::int32_t bid_qty   = 10;
    constexpr std::int32_t ask_price = 10060;
    constexpr std::int32_t ask_qty   = 7;

    std::cout << "Тестовый MD update (best bid/ask):\n"
              << "  bid " << bid_price << " x " << bid_qty << '\n'
              << "  ask " << ask_price << " x " << ask_qty << '\n';

    return 0;
}
