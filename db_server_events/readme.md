1. Таблица modules

Хранит категории/модули, к которым относятся события.

CREATE TABLE modules (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

Описание:

Колонка    	Тип	                    Описание
id	        INT             	Уникальный идентификатор модуля
name	    VARCHAR(100)	    Читаемое название модуля

Пример в C++:
enum class Module {
    snapshots_broadcaster = 1,
    tcp_server = 2,
    ...
};

2. Таблица event_types
Содержит все возможные типы событий сервера.
CREATE TABLE event_types (
    id SMALLINT PRIMARY KEY,
    module_id INT,  
    name VARCHAR(100) NOT NULL
);

Описание:
Колонка	        Тип        	        Описание
id	            SMALLINT	        Уникальный идентификатор события
module_id    	INT	                Идентификатор модуля, к которому относится событие
name	        VARCHAR(100)	    Название события

Пример в C++:
enum class EventType  {
    LOGIN = 1,
    LOGOUT = 2,
    SNAPSHOT_SENT = 3,
    ....
};

3. Таблица event_log
Главная таблица логов сервера — хранит все события.
CREATE TABLE event_log (
    id BIGSERIAL PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT NOW(),
    module_id INT NOT NULL,
    event_type_id SMALLINT NOT NULL,
    user_id INT,
    metadata JSONB
);

-- Индексы для быстрого поиска
CREATE INDEX idx_event_log_module ON event_log (module_id, timestamp); //все события по модулю
CREATE INDEX idx_event_log_type ON event_log (event_type_id, timestamp); //все события по типу
CREATE INDEX idx_event_log_user ON event_log (user_id, id);//все события по пользователю

Описание:
Колонка	Тип	Описание
id	BIGSERIAL	Уникальный идентификатор события
timestamp	TIMESTAMP	Время события (по умолчанию NOW())
module_id	INT	Модуль, к которому относится событие
event_type_id	SMALLINT	Тип события
user_id	INT	Идентификатор пользователя (если применимо)
metadata	JSONB	Дополнительные данные (например, текст ошибки, параметры операции)

Таблица создается на месяц. в начале каждого месяца создается новая таблица.(ну или иного временного интервала)


