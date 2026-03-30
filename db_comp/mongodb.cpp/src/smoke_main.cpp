#include "mongodb_connector.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    ob::MongoDbConnector::Config cfg;
    if (const char* uri = std::getenv("MONGODB_URI"); uri && *uri) {
        cfg.uri = uri;
    }
    ob::MongoDbConnector db(cfg);

    // Текст ошибки заполняется методами коннектора, если передать указатель
    std::string err;
    // внутри создаёт клиент и шлёт пинг — без живого mongod здесь будет отказ
    if (!db.connect(&err)) {
        std::cerr << "connect: " << err << "\n";
        std::cerr << " что MongoDB запущен ?\n";
        return 1;
    }
    std::cout << "Подключен.\n";
    // время в миллисекундах — удобно для сортировки и отладки
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

    // Тестовый «лучшего уровня»: цены и объёмы произвольные
    ob::BookSnapshotDoc snap;
    snap.seq = 1;       // В реальном движке seq должен расти монотонно между снимками
    snap.ts_ms = now;
    snap.bid = {10050, 10};  // лучшая цена покупки и агрегированный объём на ней
    snap.ask = {10060, 7};   // лучшая цена продажи

    // Документ попадёт в коллекцию (по умолчанию "snapshots")
    if (!db.insert_snapshot(snap, &err)) {
        std::cerr << "insert_snapshot: " << err << "\n";
        return 2;
    }
    std::cout << "Вставлен снимок =" << snap.seq << "\n";

    // Читаем один документ с максимальным seq — должны получить только что вставленный снимок.
    auto loaded = db.load_latest_snapshot(&err);
    if (!loaded) {
        std::cerr << "load_latest_snapshot: " << (err.empty() ? "пусто" : err) << "\n";
        return 3;
    }
    std::cout << "Прочитан последний снимок: bid " << loaded->bid.price << " x " << loaded->bid.qty
              << ", ask " << loaded->ask.price << " x " << loaded->ask.qty << "\n";

    return 0;
}
