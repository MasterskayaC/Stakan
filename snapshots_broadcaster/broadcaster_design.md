# Server Snapshot Broadcaster — проектирование

Broadcaster — серверный компонент микросервиса Order Book, отвечающий за рассылку **снэпшотов стакана (order book)** подключённым по TCP клиентам.

## Контекст в системе

```
                 Orders (New/Cancel/Replace)
                          │
                          ▼
┌──────────┐    ┌──────────────────┐    ┌──────────────────────┐
│  Feed    │───▶│  Order Book      │───▶│  Snapshot Broadcaster │──▶ TCP клиенты
│  Thread  │    │  Thread          │    │  Thread               │
└──────────┘    │                  │    └──────────────────────┘
   SPSC/MPSC    │  Bid book (sorted)│         ▲
                │  Ask book (sorted)│         │
                │  Best bid/ask    │─────────┘
                └──────────────────┘   MD Update / Snapshot
```

Broadcaster — это **отдельный поток**, получающий команды на рассылку через MPSC-очередь от потока Order Book и от сетевого потока (запросы клиентов на snapshot).

## Термины

| Термин | Значение |
|--------|----------|
| **Snapshot** | Полный срез текущего состояния стакана: все уровни bid/ask book |
| **MD Update** | Компактное обновление best bid/ask: `[BID_PRICE 4B][BID_QTY 4B][ASK_PRICE 4B][ASK_QTY 4B]` |
| **Broadcaster** | Поток сервера, управляющий очередью и отправкой снэпшотов/обновлений клиентам по TCP |
| **Individual send** | Заявка «отправить снэпшот пользователю N» (например, при подключении или по кнопке) |
| **Broadcast send** | Заявка «отправить снэпшот всем пользователям» |

## Входящие команды

Broadcaster принимает через MPSC-очередь:

```cpp
enum class CommandType : uint8_t {
    SendSnapshotTo,   // отправить snapshot конкретному клиенту
    SendSnapshotAll,  // отправить snapshot всем клиентам
    SendMDUpdate,     // отправить MD Update всем (best bid/ask изменился)
};

struct BroadcastCommand {
    CommandType type;
    ClientId    client_id;  // значим только для SendSnapshotTo
};
```

### Источники команд

| Источник | Команда | Когда |
|----------|---------|-------|
| Сетевой поток | `SendSnapshotTo(client_id)` | Клиент подключился / нажал кнопку «запросить snapshot» |
| Order Book поток | `SendSnapshotAll` | Крупное изменение стакана, требующее полного среза |
| Order Book поток | `SendMDUpdate` | Изменился лучший bid или ask |

## Ключевая проблема: интерференция broadcast и individual команд

Когда поступает `SendSnapshotAll`, это влияет на очередь индивидуальных отправок:

- Если в очереди стоит `SendSnapshotTo(client_id=5)`, а затем приходит `SendSnapshotAll` — пользователь 5 получит снэпшот в рамках broadcast, индивидуальная отправка **избыточна**.
- Если `SendSnapshotAll` уже в очереди, а после него приходит `SendSnapshotTo(client_id=5)` — это **новый** запрос, его нужно доставить.

### Стратегия: поглощение (absorption)

`SendSnapshotAll` **поглощает** все ожидающие `SendSnapshotTo`, поставленные **до** него:

```
Очередь до:     [SendSnapshotTo(3), SendSnapshotTo(5), SendSnapshotTo(7)]
Приходит:        SendSnapshotAll
Очередь после:  [SendSnapshotAll]

Далее:           SendSnapshotTo(5)  — новая заявка, не поглощается
Очередь:        [SendSnapshotAll, SendSnapshotTo(5)]
```

Несколько подряд идущих `SendSnapshotAll` схлопываются в один:

```
Очередь до:     [SendSnapshotAll, SendSnapshotTo(3), SendSnapshotAll]
Очередь после:  [SendSnapshotAll]
```

`SendMDUpdate` не участвует в поглощении — это другой тип сообщения (компактное обновление, а не полный срез).

## Форматы сообщений на проводе

### MD Update (best bid/ask) — 16 байт

```
Offset  Size  Field
0       4B    bid_price   (uint32)
4       4B    bid_qty     (uint32)
8       4B    ask_price   (uint32)
12      4B    ask_qty     (uint32)
```

### Snapshot — переменная длина

```
Offset  Size        Field
0       1B          message_type = SNAPSHOT (0x01)
1       4B          num_bids     (uint32)
5       4B          num_asks     (uint32)
9       num_bids×8  bid levels   (price 4B + qty 4B каждый, sorted desc by price)
...     num_asks×8  ask levels   (price 4B + qty 4B каждый, sorted asc by price)
```

## Структура модуля

```
broadcaster/
├── broadcaster.h/.cpp    // основной класс, владеет потоком и очередью
├── command_queue.h/.cpp  // очередь команд с логикой поглощения
└── snapshot.h/.cpp       // формирование snapshot из текущего состояния order book
```

### Очередь команд (`CommandQueue`)

```cpp
class CommandQueue {
public:
    void push(BroadcastCommand cmd);  // применяет правила поглощения
    std::optional<BroadcastCommand> pop();  // блокирующий или non-blocking
};
```

Правила при `push`:

| Новая команда | Действие |
|---------------|----------|
| `SendSnapshotTo(id)` | Если в очереди уже есть `SendSnapshotTo` с тем же `id` — дедупликация. Иначе — добавить в конец. |
| `SendSnapshotAll` | Удалить все `SendSnapshotTo` перед ней. Схлопнуть с предыдущим `SendSnapshotAll`, если есть. |
| `SendMDUpdate` | Добавить в конец. Схлопнуть с предыдущим `SendMDUpdate`, если есть (клиенту нужен только последний). |

### Snapshot

```cpp
struct Snapshot {
    std::vector<PriceLevel> bids;  // sorted desc by price
    std::vector<PriceLevel> asks;  // sorted asc by price
};

struct PriceLevel {
    uint32_t price;
    uint32_t quantity;
};
```

Снэпшот формируется **в момент обработки команды из очереди**, а не в момент постановки — клиент всегда получает актуальное состояние стакана.

Для чтения стакана из другого потока broadcaster обращается к Order Book через **потокобезопасный интерфейс** (shared mutex на чтение или lock-free snapshot).

### Broadcaster (основной класс)

```cpp
class Broadcaster {
    CommandQueue             queue_;        // MPSC очередь команд
    const OrderBook&         order_book_;   // ссылка на order book (read-only доступ)
    ClientRegistry&          clients_;      // реестр подключённых TCP-клиентов
    std::thread              thread_;

public:
    void enqueue(BroadcastCommand cmd);     // вызывается из других потоков
    void run();                             // цикл обработки в своём потоке

private:
    void handleSendSnapshotTo(ClientId id);
    void handleSendSnapshotAll();
    void handleSendMDUpdate();

    Snapshot buildSnapshot();               // читает order book, формирует срез
    void     sendToClient(ClientId id, const void* data, size_t len);
};
```

## Порядок обработки

```
1. Команда поступает в CommandQueue (из потока Order Book или сетевого потока)
2. CommandQueue применяет правила поглощения/дедупликации
3. Поток Broadcaster берёт команду из головы очереди
4. В зависимости от типа:
   a) SendSnapshotTo(id):
      - buildSnapshot() из текущего состояния order book
      - сериализовать в бинарный формат
      - отправить по TCP клиенту id
   b) SendSnapshotAll:
      - buildSnapshot()
      - сериализовать
      - отправить всем клиентам из ClientRegistry
   c) SendMDUpdate:
      - прочитать текущие best bid/ask из order book
      - сформировать 16-байтный MD Update
      - отправить всем клиентам
5. Перейти к следующей команде
```

## Взаимодействие с другими потоками

```
Feed Thread ──SPSC──▶ Order Book Thread ──MPSC──▶ Broadcaster Thread
                                                        │
                           Network Thread ──MPSC────────┘
                           (клиент запросил snapshot)    │
                                                        ▼
                                                   TCP send()
                                                   к клиентам
```

| Очередь | Тип | Откуда → Куда |
|---------|-----|---------------|
| Feed → OrderBook | SPSC | Feed Thread → OrderBook Thread |
| OrderBook → Broadcaster | MPSC | OrderBook Thread → Broadcaster Thread |
| Network → Broadcaster | MPSC | Network Thread → Broadcaster Thread |

## Гарантии

- **Актуальность**: снэпшот строится в момент отправки, не в момент постановки команды в очередь.
- **Без дублей**: поглощение устраняет избыточные отправки при наложении individual и broadcast команд.
- **Порядок**: команды после `SendSnapshotAll` обрабатываются после него — клиент сначала получит broadcast, потом индивидуальное обновление.
- **Потокобезопасность**: доступ к order book для чтения через shared mutex; очереди — lock-free или с мьютексом.
