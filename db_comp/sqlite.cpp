#include <sqlite3.h>
#include <iostream>
#include <string>

// ПЛЮСЫ SQLite:
// - Не надо поднимать сервер, файл создается автоматом
// - Можно запускать в CI/CD без docker
// - Быстро пересоздать базу просто удалить .db файл
// - ACID транзакции, WAL-режим для конкурентности
// - Микросекундные задержки (in-process)
// - Библиотека ~1MB, zero-config

// МИНУСЫ SQLite:
// - Один процесс пишет, остальные ждут
// - Нет горизонтального масштабирования
// - Нет сетевого доступа (только локально)

int main() {
    sqlite3* db = nullptr;
    char* errmsg = nullptr;

    // 1. ОТКРЫТЬ/СОЗДАТЬ БД (файл test.db)
    sqlite3_open("test.db", &db);

    // 2. ВКЛЮЧИТЬ WAL (Write-Ahead Logging) - чтение не блокирует запись
    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errmsg);

    // 3. СОЗДАТЬ ТАБЛИЦУ
    const char* create_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            balance REAL DEFAULT 0
        );
    )";
    sqlite3_exec(db, create_sql, nullptr, nullptr, &errmsg);

    // 4. ВСТАВИТЬ ДАННЫЕ
    const char* insert_sql = "INSERT INTO users (name, balance) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "Alice", -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, 1000.50);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // 5. ТРАНЗАКЦИЯ (пакетная вставка - в 100x быстрее)
    sqlite3_exec(db, "BEGIN;", nullptr, nullptr, &errmsg);
    for (int i = 0; i < 10; ++i) {
        std::string name = "User" + std::to_string(i);
        sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, i * 100.0);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errmsg);

    // 6. ЗАПРОС С ПАРАМЕТРАМИ
    const char* select_sql = "SELECT id, name, balance FROM users WHERE balance > ?;";
    sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr);
    sqlite3_bind_double(stmt, 1, 500.0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        double balance = sqlite3_column_double(stmt, 2);
        std::cout << id << " | " << name << " | " << balance << std::endl;
    }
    sqlite3_finalize(stmt);

    // 7. ОБНОВЛЕНИЕ
    const char* update_sql = "UPDATE users SET balance = balance + 100 WHERE name = ?;";
    sqlite3_prepare_v2(db, update_sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "Alice", -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // 8. УДАЛЕНИЕ
    const char* delete_sql = "DELETE FROM users WHERE balance < ?;";
    sqlite3_prepare_v2(db, delete_sql, -1, &stmt, nullptr);
    sqlite3_bind_double(stmt, 1, 50.0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // 9. МЕТАДАННЫЕ: размер БД
    sqlite3_prepare_v2(db, "PRAGMA page_count;", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    int64_t pages = sqlite3_column_int64(stmt, 0);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, "PRAGMA page_size;", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    int64_t page_size = sqlite3_column_int64(stmt, 0);
    sqlite3_finalize(stmt);

    std::cout << "DB size: " << (pages * page_size) / 1024.0 << " KB" << std::endl;

    // 10. ОПТИМИЗАЦИЯ (периодически)
    sqlite3_exec(db, "VACUUM;", nullptr, nullptr, &errmsg);

    // 11. ЗАКРЫТЬ
    sqlite3_close(db);

    return 0;
}
