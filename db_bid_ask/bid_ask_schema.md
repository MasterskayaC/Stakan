# Схема таблиц для OrderBook
## 1. Таблица ордеров (текущее состояние)
```
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY,
    side_id INTEGER NOT NULL,      -- 1 = bid, 2 = ask
    price INTEGER NOT NULL,        -- тики
    quantity INTEGER NOT NULL,
    created_at INTEGER NOT NULL
);
```
**описание колонок:**
| Колонка      | Тип     | Описание                                    |
| ------------ | ------- | ------------------------------------------- |
| `id`         | INTEGER | Уникальный идентификатор ордера (Order::id) |
| `side_id`    | INTEGER | Сторона ордера: 1 = bid, 2 = ask            |
| `price`      | INTEGER | Цена ордера в тиках                         |
| `quantity`   | INTEGER | Объём ордера                                |
| `created_at` | INTEGER | Время создания ордера (Unix timestamp)      |

### 1.1. Индексы - аналоги компараторов
*Имитируем BidComparator и AskComparator*
```
-- для bid 
CREATE INDEX idx_bids 
ON orders (price DESC, quantity DESC)
WHERE side_id = 1;

-- для ask
CREATE INDEX idx_asks 
ON orders (price ASC, quantity DESC)
WHERE side_id = 2;

-- быстрый доступ по id
CREATE INDEX idx_orders_id ON orders (id);
```
### 1.2. Таблица типов ордеров
```
CREATE TABLE order_type (
    id INTEGER PRIMARY KEY,
    description TEXT NOT NULL
);
```
**Описание колонок:
| Колонка       | Тип     | Описание                                |
| ------------- | ------- | --------------------------------------- |
| `id`          | INTEGER | Идентификатор типа события              |
| `description` | TEXT    | Тип события: `NEW`, `CANCEL`, `REPLACE` |

### 1.3. Таблица сторон ордеров
```
CREATE TABLE IF NOT EXISTS order_side (
    id INTEGER PRIMARY KEY,
    description TEXT NOT NULL
);
```
**Описание колонок:**
| Колонка       | Тип     | Описание                            |
| ------------- | ------- | ----------------------------------- |
| `id`          | INTEGER | Идентификатор стороны ордера        |
| `description` | TEXT    | Текстовое описание: `bid` или `ask` |

## 2. Таблица изменений
```
CREATE TABLE IF NOT EXISTS order_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,

    type_id INTEGER NOT NULL,
    order_id INTEGER,

    side_id INTEGER,              -- 1/2
    price INTEGER,                -- ticks
    quantity INTEGER,

    metadata TEXT
);
```
### 2.1. Индексы
```
CREATE INDEX IF NOT EXISTS idx_events_order_id
ON order_events (order_id);

CREATE INDEX IF NOT EXISTS idx_events_timestamp
ON order_events (timestamp);

CREATE INDEX IF NOT EXISTS idx_events_type
ON order_events (type_id);
```
**описание колонок:**
| Колонка     | Тип     | Описание                                |
| ----------- | ------- | --------------------------------------- |
| `id`        | INTEGER | Уникальный идентификатор события        |
| `timestamp` | INTEGER | Время события (Unix timestamp)          |
| `type_id`   | INTEGER | Тип события (FK → `order_type.id`)      |
| `order_id`  | INTEGER | Идентификатор ордера                    |
| `side_id`   | INTEGER | Сторона ордера (1 = bid, 2 = ask)       |
| `price`     | INTEGER | Цена ордера в момент события (тики)     |
| `quantity`  | INTEGER | Количество в момент события             |
| `metadata`  | TEXT    | Дополнительные данные (JSON или строка) |

## 3. Таблица снапшотов
```
CREATE TABLE snapshots (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,

    -- быстрый доступ к top of book
    best_bid_price INTEGER NOT NULL,
    best_bid_quantity INTEGER NOT NULL,
    best_ask_price INTEGER NOT NULL,
    best_ask_quantity INTEGER NOT NULL,

    -- храним сериализованный снапшот
    book BLOB NOT NULL
);

-- быстрый доступ к последним снапшотам
CREATE INDEX idx_snapshots_timestamp
ON snapshots (timestamp);
```
**описание колонок:**
| Колонка             | Тип     | Описание                                 |
| ------------------- | ------- | ---------------------------------------- |
| `id`                | INTEGER | Уникальный идентификатор снапшота        |
| `timestamp`         | INTEGER | Время создания снапшота (Unix timestamp) |
| `best_bid_price`    | INTEGER | Лучшая цена покупки (тики)               |
| `best_bid_quantity` | INTEGER | Объём лучшего bid                        |
| `best_ask_price`    | INTEGER | Лучшая цена продажи (тики)               |
| `best_ask_quantity` | INTEGER | Объём лучшего ask                        |
| `book`              | BLOB    | Сериализованный полный снимок стакана    |

## Использование (примеры):
### Таблицы сторон и типов:
```
INSERT INTO order_side VALUES
(1, 'bid'),
(2, 'ask');
```
```
INSERT INTO order_type (id, description) VALUES
(1, 'NEW'),
(2, 'CANCEL'),
(3, 'REPLACE');
```
### New bid/ask:
```
INSERT INTO orders (id, side_id, price, quantity, created_at)
VALUES (1, 1, 1005, 10, 1710000000);
```
*сразу отмечаем в ивентах:*
```
INSERT INTO order_events (timestamp, type_id, order_id, side_id, price, quantity)
VALUES (1710000000, 1, 1, 1, 1005, 10);
```
### Лучший bid/ask:
```
SELECT *
FROM orders
WHERE side = 1 --или 2 (ask)
ORDER BY price DESC, quantity DESC
LIMIT 1;
```

### Cancel bid/ask:
```
-- удаляем из текущего состояния
DELETE FROM orders
WHERE id = 2;

-- записываем ивент
INSERT INTO order_events (timestamp, type_id, order_id, side_id)
VALUES (1710000020, 2, 2, 1);
```
### Replace bid/ask:
```
-- обновляем текущий ордер
UPDATE orders
SET price = 1020,
    quantity = 7
WHERE id = 1;

-- логируем изменение
INSERT INTO order_events (timestamp, type, order_id, side, price, quantity)
VALUES (1710000030, 3, 1, 1, 1020, 7);
```
### Snapshots:
```
// Логика:
INSERT INTO snapshots (
    timestamp,
    best_bid_price,
    best_bid_quantity,
    best_ask_price,
    best_ask_quantity,
    book
)
VALUES (?, ?, ?, ?, ?, ?);

// Например:
sqlite3_stmt* stmt;

sqlite3_prepare_v2(db,
    "INSERT INTO snapshots (timestamp, best_bid_price, best_bid_quantity, best_ask_price, best_ask_quantity, book) "
    "VALUES (?, ?, ?, ?, ?, ?);",
    -1, &stmt, nullptr);

// допустим есть snapshot
auto now = std::time(nullptr);

// топ уровни
auto bestBid = snapshot.topBids[0];
auto bestAsk = snapshot.topAsks[0];

// Сериализация:
auto blob = snapshot.serialize();

// биндим параметры:
sqlite3_bind_int64(stmt, 1, now);
sqlite3_bind_int(stmt, 2, bestBid.price);
sqlite3_bind_int(stmt, 3, bestBid.quantity);
sqlite3_bind_int(stmt, 4, bestAsk.price);
sqlite3_bind_int(stmt, 5, bestAsk.quantity);

// BLOB
sqlite3_bind_blob(stmt, 6, blob.data(), blob.size(), SQLITE_TRANSIENT);

// выполняем
sqlite3_step(stmt);
sqlite3_finalize(stmt);

//  получить полный снапшот:
SELECT book
FROM snapshots
ORDER BY timestamp DESC
LIMIT 1;
// или за интервал:
SELECT timestamp, best_bid_price, best_ask_price
FROM snapshots
WHERE timestamp BETWEEN 1710000000 AND 1710001000
ORDER BY timestamp;
```
## Скрипт для создания таблицы:
```
-- 1. order_side (справочник)

CREATE TABLE IF NOT EXISTS order_side (
    id INTEGER PRIMARY KEY,
    description TEXT NOT NULL
);

INSERT INTO order_side (id, description) VALUES
(1, 'bid'),
(2, 'ask');


-- 2. order_type (справочник событий)

CREATE TABLE IF NOT EXISTS order_type (
    id INTEGER PRIMARY KEY,
    description TEXT NOT NULL
);

INSERT INTO order_type (id, description) VALUES
(1, 'NEW'),
(2, 'CANCEL'),
(3, 'REPLACE');


-- 3. orders (текущее состояние стакана)

CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY,
    side_id INTEGER NOT NULL,
    price INTEGER NOT NULL,      -- ticks
    quantity INTEGER NOT NULL,
    created_at INTEGER NOT NULL  -- Unix timestamp
);

-- Bid index (price desc)
CREATE INDEX IF NOT EXISTS idx_orders_bid
ON orders (price DESC, quantity DESC)
WHERE side_id = 1;

-- Ask index (price asc)
CREATE INDEX IF NOT EXISTS idx_orders_ask
ON orders (price ASC, quantity DESC)
WHERE side_id = 2;

-- lookup by id
CREATE INDEX IF NOT EXISTS idx_orders_id
ON orders (id);


-- 4. order_events (история)

CREATE TABLE IF NOT EXISTS order_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,

    type_id INTEGER NOT NULL,
    order_id INTEGER,

    side_id INTEGER,
    price INTEGER,
    quantity INTEGER,

    metadata TEXT
);

-- fast lookup by order
CREATE INDEX IF NOT EXISTS idx_events_order_id
ON order_events (order_id);

-- time scan
CREATE INDEX IF NOT EXISTS idx_events_timestamp
ON order_events (timestamp);

-- filter by event type
CREATE INDEX IF NOT EXISTS idx_events_type
ON order_events (type_id);


-- 5. snapshots (state checkpoints)

CREATE TABLE IF NOT EXISTS snapshots (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,

    best_bid_price INTEGER NOT NULL,
    best_bid_quantity INTEGER NOT NULL,
    best_ask_price INTEGER NOT NULL,
    best_ask_quantity INTEGER NOT NULL,

    book BLOB NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_snapshots_timestamp
ON snapshots (timestamp);
```
