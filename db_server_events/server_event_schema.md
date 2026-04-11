# 📝 Схема таблиц для логирования событий сервера

## 1. Таблица `modules`
Хранит категории/модули, к которым относятся события.

```sql
CREATE TABLE modules (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);
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

```sql
CREATE TABLE event_types (
    id SMALLINT PRIMARY KEY,
    module_id INT,
    name VARCHAR(100) NOT NULL
);
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

```sql
CREATE TABLE event_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    module_id INTEGER NOT NULL,
    event_type_id INTEGER NOT NULL,
    user_id INTEGER,
    metadata TEXT
);


-- Индексы для быстрого поиска
CREATE INDEX idx_event_log_module ON event_log (module_id, timestamp);  -- все события по модулю
CREATE INDEX idx_event_log_type ON event_log (event_type_id, timestamp); -- все события по типу
CREATE INDEX idx_event_log_user ON event_log (user_id, id);             -- все события по пользователю
```

**Описание колонок:**

| Колонка | Тип | Описание |
|---------|-----|----------|
| `id` | BIGSERIAL | Уникальный идентификатор события |
| `timestamp` | TIMESTAMP | Время события (по умолчанию NOW()) |
| `module_id` | INT | Модуль, к которому относится событие |
| `event_type_id` | SMALLINT | Тип события |
| `user_id` | INT | Идентификатор пользователя (если применимо) |
| `metadata` | JSONB | Дополнительные данные (например, текст ошибки, параметры операции) |


**Особенности:**

- Индексы ускоряют выборку по модулю, типу и пользователю.
- `metadata` позволяет хранить любые дополнительные данные без изменения схемы.
- Таблица создается на месяц (или другой временной интервал) для упрощения архивации и управления размером логов.

