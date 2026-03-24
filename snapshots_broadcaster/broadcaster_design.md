# Server Snapshot Broadcaster — проектирование

Broadcaster — серверный компонент микросервиса Order Book, отвечающий за рассылку **снэпшотов стакана** подключённым по TCP клиентам.

```

Broadcaster — это **отдельный поток**, получающий команды на рассылку через MPSC-очередь от потока Order Book и от сетевого потока (запросы клиентов на snapshot).


## Входящие команды

Broadcaster принимает через MPSC-очередь:

```cpp
enum class CommandType : uint8_t {
    SendSnapshotTo,   // отправить snapshot конкретному клиенту
    SendSnapshotAll,  // отправить snapshot всем 
    SendMDUpdate,     // отправить MD Update всем 
};

struct BroadcastCommand {
    CommandType type;
    ClientId    client_id;  // значим только для SendSnapshotTo
};
```

Команды в очереди содержат только **намерение выполнить рассылку**, а не сами данные.
Снэпшот/обновление формируется **в момент обработки команды** в потоке broadcaster.


## Ключевая проблема: интерференция broadcast и individual команд

Когда поступает `SendSnapshotAll`, это влияет на очередь индивидуальных отправок:

- Если в очереди стоит `SendSnapshotTo(client_id=5)`, а затем приходит `SendSnapshotAll` — пользователь 5 получит снэпшот в рамках broadcast, индивидуальная отправка **избыточна**.
- Если `SendSnapshotAll` уже в очереди, а после него приходит `SendSnapshotTo(client_id=5)` — это **новый** запрос, его нужно доставить.

### Стратегия: поглощение 

`SendSnapshotAll` **поглощает** все ожидающие `SendSnapshotTo`, поставленные **до** него:
`SendMDUpdate` не участвует в поглощении — это другой тип сообщения

Важно: `SendSnapshotAll` — это отдельная операция "сделать broadcast", она **не раскладывается** на набор команд `SendSnapshotTo` в очереди.
При обработке `SendSnapshotAll` broadcaster строит snapshot **один раз** и затем отправляет его всем текущим подключённым клиентам.

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

### Snapshot

```cpp
struct Snapshot {
    std::vector<PriceLevel> bids;  
    std::vector<PriceLevel> asks;  
};

struct PriceLevel {
    uint32_t price;
    uint32_t quantity;
};
```

Снэпшот формируется **один раз в момент обработки команды из очереди**, а не в момент постановки — все клиенты в рамках одной команды получают **одинаковый** актуальный срез стакана.

Для чтения стакана из другого потока broadcaster обращается к Order Book через **потокобезопасный интерфейс**.

### MD Update

Команда `SendMDUpdate` означает "разослать обновление" и не предполагает, что broadcaster сам вычисляет инкремент (дельту) состояния.
Формирование данных для MD update находится вне broadcaster (например, в `OrderBook` или отдельном компоненте), а broadcaster отвечает только за рассылку.

### Broadcaster

```cpp
class Broadcaster {
    CommandQueue             queue_;        // MPSC очередь команд
    const OrderBook&         order_book_;   // ссылка на order book 
    ClientRegistry&          clients_;      // реестр подключённых TCP-клиентов
    std::thread              thread_;

public:
    void enqueue(BroadcastCommand cmd);     // вызывается из других потоков
    void run();                             // цикл обработки в своём потоке

private:
    void handleSendSnapshotTo(ClientId id);
    void handleSendSnapshotAll();
    void handleSendMDUpdate();

    Snapshot buildSnapshot();               // читает order book, формирует супер снэпшот срез
    void     sendToClient(ClientId id, const void* data, size_t len);
};
```

