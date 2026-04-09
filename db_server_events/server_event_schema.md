# 📝 Схема таблиц для логирования событий сервера

## 1. Таблица `modules`
Хранит категории/модули, к которым относятся события.
создание таблицы
```sql
CREATE TABLE modules (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);
```
вставка в таблицу
```sql
INSERT INTO modules (id, name)
VALUES (1, 'snapshots_broadcaster');
```
**Описание колонок:**

| Колонка | Тип | Описание |
|---------|-----|----------|
| `id` | INT | Уникальный идентификатор модуля |
| `name` | VARCHAR(100) | Читаемое название модуля |

**Пример в C++:**

```cpp
enum class Module {
    snapshots_broadcaster = 1,
    tcp_server = 2,
    // ...
};
```

---

## 2. Таблица `event_types`
Содержит все возможные типы событий сервера.
создание таблицы
```sql
CREATE TABLE event_types (
    id SMALLINT PRIMARY KEY,
    module_id INT,
    name VARCHAR(100) NOT NULL
);
```
вставка в таблицу
```sql
INSERT INTO event_types (id, module_id, name)
VALUES (1, 1, 'user_login');
```
**Описание колонок:**

| Колонка | Тип | Описание |
|---------|-----|----------|
| `id` | SMALLINT | Уникальный идентификатор события |
| `module_id` | INT | Идентификатор модуля, к которому относится событие |
| `name` | VARCHAR(100) | Название события |

**Пример в C++:**

```cpp
enum class EventType {
    LOGIN = 1,
    LOGOUT = 2,
    SNAPSHOT_SENT_ALL = 3,
    SNAPSHOT_SENT_TO = 4,
    MD_UPDATED = 5,
    START_SEVER = 6,
    STOP_SERVER = 7,

    // ...
};
```

---

## 3. Таблица `event_log`
Главная таблица логов сервера — хранит все события.
Создание таблицы
```sql
CREATE TABLE event_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL DEFAULT (CAST(strftime('%s','now') AS INTEGER)),
    module_id INTEGER NOT NULL,
    event_type_id INTEGER NOT NULL,
    user_id INTEGER,
    metadata TEXT
);

Создание индексов
-- Индексы для быстрого поиска
CREATE INDEX idx_event_log_module ON event_log (module_id, timestamp);  -- все события по модулю
CREATE INDEX idx_event_log_type ON event_log (event_type_id, timestamp); -- все события по типу
CREATE INDEX idx_event_log_user ON event_log (user_id, id);             -- все события по пользователю
```
Вставка в таблицу события с user_id
```sql
INSERT INTO event_log (module_id, event_type_id, user_id, metadata)
VALUES (1, 2, 1001, '{"action":"login"}');
```
Вставка в таблицу события без user_id
```sql
INSERT INTO event_log (module_id, event_type_id, metadata)
VALUES (1, 2, '{"action":"login"}');
```
поиск по таблице 
все события модуля,
```sql
SELECT *
FROM event_log
WHERE module_id = 1
ORDER BY timestamp DESC;
```
все события по типу
```sql
SELECT *
FROM event_log
WHERE event_type_id = 2
ORDER BY timestamp DESC;
```
все события по пользователю внутри определенного интервала времени
```sql
SELECT *
FROM event_log
WHERE timestamp >= 1710000000
  AND timestamp <= 1713000000
ORDER BY timestamp DESC;
```

**Описание колонок:**

| Колонка | Тип | Описание |
|---------|-----|----------|
| `id` | INTEGER | Уникальный идентификатор события |
| `timestamp` | INTEGER | кол-во секунд с 01.01.1970 до времени события (по умолчанию NOW()) |
| `module_id` | INT | Модуль, к которому относится событие |
| `event_type_id` | SMALLINT | Тип события |
| `user_id` | INT | Идентификатор пользователя (если применимо) |
| `metadata` | TEXT | Дополнительные данные (например, текст ошибки, параметры операции) |


**Особенности:**

- Индексы ускоряют выборку по модулю, типу и пользователю.
- `metadata` позволяет хранить любые дополнительные данные без изменения схемы.
- Таблица создается на месяц (или другой временной интервал) для упрощения архивации и управления размером логов.

