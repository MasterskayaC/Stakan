# Схема таблиц для OrderBook
## 1. Таблица ордеров (текущее состояние)
```
CREATE TABLE orders (
    id INTEGER PRIMARY KEY,
    side TEXT NOT NULL, -- 'bid' или 'ask'
    price REAL NOT NULL,
    quantity REAL NOT NULL,
    created_at INTEGER NOT NULL
);
```
**описание колонок:**
| Колонка      | Тип     | Описание                                                    |
| ------------ | ------- | ----------------------------------------------------------- |
| `id`         | INTEGER | Уникальный идентификатор ордера (соответствует `Order::id`) |
| `side`       | TEXT    | Сторона ордера: `bid` или `ask`                             |
| `price`      | REAL    | Цена ордера                                                 |
| `quantity`   | REAL    | Объём (количество)                                          |
| `created_at` | INTEGER | Время создания ордера (Unix timestamp)                      |

### 1.1. Индексы - аналоги компараторов
*Имитируем BidComparator и AskComparator*
```
-- для bid 
CREATE INDEX idx_bids 
ON orders (price DESC, quantity DESC)
WHERE side = 'bid';

-- для ask
CREATE INDEX idx_asks 
ON orders (price ASC, quantity DESC)
WHERE side = 'ask';

-- быстрый доступ по id
CREATE INDEX idx_orders_id ON orders (id);
```
## 2. Таблица изменений
```
CREATE TABLE order_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,
    type TEXT NOT NULL, -- 'NEW', 'CANCEL', 'REPLACE'
    order_id INTEGER,
    side TEXT,
    price REAL,
    quantity REAL,
    metadata TEXT
);
```
**описание колонок:**
| Колонка     | Тип     | Описание                                                      |
| ----------- | ------- | ------------------------------------------------------------- |
| `id`        | INTEGER | Уникальный идентификатор события                              |
| `timestamp` | INTEGER | Время события (Unix timestamp)                                |
| `type`      | TEXT    | Тип события: `NEW`, `CANCEL`, `REPLACE`                       |
| `order_id`  | INTEGER | Идентификатор ордера, к которому относится событие            |
| `side`      | TEXT    | Сторона ордера (`bid` / `ask`) на момент события              |
| `price`     | REAL    | Цена ордера в момент события                                  |
| `quantity`  | REAL    | Количество в момент события                                   |
| `metadata`  | TEXT    | Дополнительные данные (JSON-строка: причина, источник и т.п.) |

## 3. Таблица снапшотов
```
CREATE TABLE snapshots (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,
    best_bid_price REAL,
    best_bid_quantity REAL,
    best_ask_price REAL,
    best_ask_quantity REAL
);
```
**описание колонок:**
| Колонка             | Тип     | Описание                                 |
| ------------------- | ------- | ---------------------------------------- |
| `id`                | INTEGER | Уникальный идентификатор снапшота        |
| `timestamp`         | INTEGER | Время создания снапшота (Unix timestamp) |
| `best_bid_price`    | REAL    | Лучшая цена покупки (Bid)                |
| `best_bid_quantity` | REAL    | Объём лучшего Bid                        |
| `best_ask_price`    | REAL    | Лучшая цена продажи (Ask)                |
| `best_ask_quantity` | REAL    | Объём лучшего Ask                        |
## Использование (примеры):
### New bid/ask:
```
INSERT INTO orders (id, side, price, quantity, created_at)
VALUES (1, 'bid', 100.5, 10, 1710000000);
```
*сразу отмечаем в ивентах:*
```
INSERT INTO order_events (timestamp, type, order_id, side, price, quantity)
VALUES (1710000000, 'NEW', 1, 'bid', 100.5, 10); --или ask
```
### Лучший bid/ask:
```
SELECT *
FROM orders
WHERE side = 'bid' --или ask
ORDER BY price DESC, quantity DESC
LIMIT 1;
```

### Cancel bid/ask:
```
-- удаляем из текущего состояния
DELETE FROM orders
WHERE id = 2;

-- записываем ивент
INSERT INTO order_events (timestamp, type, order_id, side)
VALUES (1710000020, 'CANCEL', 2, 'bid');
```
### Replace bid/ask:
```
-- обновляем текущий ордер
UPDATE orders
SET price = 102.0,
    quantity = 7
WHERE id = 1;

-- логируем изменение
INSERT INTO order_events (timestamp, type, order_id, side, price, quantity)
VALUES (1710000030, 'REPLACE', 1, 'bid', 102.0, 7);
```

## Скрипт для создания таблицы:
```
-- 1. Таблица orders

CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY, -- order_id
    side TEXT NOT NULL CHECK (side IN ('bid','ask')),
    price REAL NOT NULL,
    quantity REAL NOT NULL,
    created_at INTEGER NOT NULL -- Unix timestamp
);

-- Индекс для bid
CREATE INDEX IF NOT EXISTS idx_bids
ON orders (price DESC, quantity DESC)
WHERE side = 'bid';

-- Индекс для ask
CREATE INDEX IF NOT EXISTS idx_asks
ON orders (price ASC, quantity DESC)
WHERE side = 'ask';

-- 2. Таблица order_events

CREATE TABLE IF NOT EXISTS order_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('NEW','CANCEL','REPLACE')),
    order_id INTEGER,
    side TEXT CHECK (side IN ('bid','ask')),
    price REAL,
    quantity REAL,
    metadata TEXT,

    FOREIGN KEY (order_id) REFERENCES orders(id)
);

-- Индексы для быстрых выборок

CREATE INDEX IF NOT EXISTS idx_events_order_id
ON order_events (order_id);

CREATE INDEX IF NOT EXISTS idx_events_timestamp
ON order_events (timestamp);

-- 3. Таблица snapshots

CREATE TABLE IF NOT EXISTS snapshots (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,
    best_bid_price REAL,
    best_bid_quantity REAL,
    best_ask_price REAL,
    best_ask_quantity REAL
);

-- Индекс по времени (последние снепшоты)
CREATE INDEX IF NOT EXISTS idx_snapshots_timestamp
ON snapshots (timestamp);
```
