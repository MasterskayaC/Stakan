#include <pqxx/pqxx>
#include <iostream>

// ПЛЮСЫ PostgreSQL:
// - Полноценный SQL
// - Полная ACID атомарность, согласованность, изоляция, долговечность
// - Расширяемость
// - Горизонтальное масштабирование
// - Безопасность

// МИНУСЫ PostgreSQL:
// - Требует установки и настройки сервера
// - Для тестов тяжелее, чем SQLite (нужен отдельный процесс)

int main() {
    try {
        // 1. ПОДКЛЮЧЕНИЕ
        pqxx::connection conn("host=localhost port=5432 dbname=testdb user=postgres password=postgres");
        std::cout << "✓ Подключено к PostgreSQL" << std::endl;

        // 2. СОЗДАТЬ ТАБЛИЦУ
        pqxx::work tx_create(conn);
        tx_create.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id SERIAL PRIMARY KEY,"
            "name TEXT NOT NULL,"
            "score INT DEFAULT 0,"
            "created_at TIMESTAMPTZ DEFAULT NOW())"
        );
        tx_create.commit();
        std::cout << "✓ Таблица создана" << std::endl;

        // 3. ВСТАВКА ДАННЫХ
        pqxx::work tx_insert(conn);
        tx_insert.exec_params("INSERT INTO users (name, score) VALUES ($1, $2)", "Alice", 100);
        tx_insert.exec_params("INSERT INTO users (name, score) VALUES ($1, $2)", "Bob", 200);
        tx_insert.exec_params("INSERT INTO users (name, score) VALUES ($1, $2)", "Charlie", 50);
        tx_insert.commit();
        std::cout << "✓ Данные вставлены" << std::endl;

        // 4. SELECT С УСЛОВИЕМ
        pqxx::work tx_select(conn);
        pqxx::result res = tx_select.exec_params(
            "SELECT id, name, score FROM users WHERE score > $1 ORDER BY score DESC",
            100
        );

        std::cout << "\n=== Users with score > 100 ===" << std::endl;
        for (const auto& row : res) {
            std::cout << row[0].as<int>() << " | "
                      << row[1].as<std::string>() << " | "
                      << row[2].as<int>() << std::endl;
        }

        // 5. АГРЕГАЦИЯ (COUNT, AVG)
        pqxx::result agg = tx_select.exec("SELECT COUNT(*), AVG(score), MIN(score), MAX(score) FROM users");
        std::cout << "\n=== Статистика ===" << std::endl;
        std::cout << "Всего: " << agg[0][0].as<int>() << std::endl;
        std::cout << "Средний score: " << agg[0][1].as<double>() << std::endl;
        std::cout << "Min: " << agg[0][2].as<int>() << ", Max: " << agg[0][3].as<int>() << std::endl;

        // 6. ОКОННАЯ ФУНКЦИЯ (ранжирование)
        pqxx::result ranked = tx_select.exec(
            "SELECT name, score, "
            "RANK() OVER (ORDER BY score DESC) as rank "
            "FROM users"
        );
        std::cout << "\n=== Рейтинг (оконная функция) ===" << std::endl;
        for (const auto& row : ranked) {
            std::cout << row[0].as<std::string>() << " | "
                      << row[1].as<int>() << " | rank: "
                      << row[2].as<int>() << std::endl;
        }

        tx_select.commit();

        // 7. ТРАНЗАКЦИЯ (ACID: атомарность)
        pqxx::work tx_transaction(conn);
        try {
            tx_transaction.exec_params("UPDATE users SET score = score + 10 WHERE name = $1", "Alice");
            tx_transaction.exec_params("UPDATE users SET score = score + 10 WHERE name = $1", "Bob");
            tx_transaction.commit();  // Только если обе операции успешны
            std::cout << "\n✓ Транзакция: +10 баллов Alice и Bob" << std::endl;
        } catch (...) {
            tx_transaction.abort();  // Откат при ошибке
            std::cout << "✗ Транзакция откачена" << std::endl;
        }

        // 8. JSONB (гибкость NoSQL внутри реляционной БД)
        pqxx::work tx_json(conn);
        tx_json.exec(
            "CREATE TABLE IF NOT EXISTS metadata ("
            "id SERIAL PRIMARY KEY,"
            "data JSONB)"
        );

        tx_json.exec_params(
            "INSERT INTO metadata (data) VALUES ($1)",
            R"({"source": "test", "version": 1, "tags": ["postgres", "jsonb"]})"
        );

        pqxx::result json_res = tx_json.exec(
            "SELECT data->>'source' as source, "
            "jsonb_array_length(data->'tags') as tags_count "
            "FROM metadata"
        );

        std::cout << "\n=== JSONB данные ===" << std::endl;
        for (const auto& row : json_res) {
            std::cout << "source: " << row[0].as<std::string>()
                      << ", tags count: " << row[1].as<int>() << std::endl;
        }
        tx_json.commit();

        // 9. ОБНОВЛЕНИЕ И УДАЛЕНИЕ
        pqxx::work tx_update(conn);
        tx_update.exec("UPDATE users SET score = score + 50 WHERE name = 'Charlie'");
        tx_update.exec("DELETE FROM users WHERE score < 100");
        tx_update.commit();
        std::cout << "✓ Обновлено: Charlie +50, удалены с score < 100" << std::endl;

        // 10. ФИНАЛЬНЫЙ SELECT
        pqxx::work tx_final(conn);
        pqxx::result final = tx_final.exec("SELECT name, score FROM users ORDER BY score DESC");
        std::cout << "\n=== Итоговые пользователи ===" << std::endl;
        for (const auto& row : final) {
            std::cout << row[0].as<std::string>() << " | " << row[1].as<int>() << std::endl;
        }
        tx_final.commit();

        std::cout << "\n✓ Работа завершена" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
