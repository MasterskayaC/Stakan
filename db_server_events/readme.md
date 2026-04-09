1. Таблица modules

Хранит категории/модули, к которым относятся события.

CREATE TABLE modules (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

Описание:

Колонка	Тип	Описание
id	INT	Уникальный идентификатор модуля
name	VARCHAR(100)	Читаемое название модуля

Пример в C++:

enum class Module {
    AUTH = 1,
    FILES = 2,
    NOTIFICATIONS = 3,
};
2. Таблица event_types

Содержит все возможные типы событий сервера.

CREATE TABLE event_types (
    id SMALLINT PRIMARY KEY,
    module_id INT,  
    name VARCHAR(100) NOT NULL
);

Описание:

Колонка	Тип	Описание
id	SMALLINT	Уникальный идентификатор события
module_id	INT	Идентификатор модуля, к которому относится событие
name	VARCHAR(100)	Название события

Пример в C++:

enum class EventType : uint16_t {
    LOGIN = 1,
    LOGOUT = 2,
    FILE_UPLOAD = 3,
    FILE_DELETE = 4,
};

🔹 Связь с модулем и событием хранится в коде через enum. Таблица нужна для аналитики/отчетов, FK необязателен.

3. Таблица event_log

Главная таблица логов сервера — хранит все события.

CREATE TABLE event_log (
    id BIGSERIAL PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT NOW(),
    module_id INT NOT NULL,
    event_type_id SMALLINT NOT NULL,
    user_id INT,
    session_id UUID,
    metadata JSONB
);

-- Индексы для быстрого поиска
CREATE INDEX idx_event_log_module ON event_log (module_id, timestamp);
CREATE INDEX idx_event_log_type ON event_log (event_type_id, timestamp);
CREATE INDEX idx_event_log_user ON event_log (user_id, id);

Описание:

Колонка	Тип	Описание
id	BIGSERIAL	Уникальный идентификатор события
timestamp	TIMESTAMP	Время события (по умолчанию NOW())
module_id	INT	Модуль, к которому относится событие
event_type_id	SMALLINT	Тип события
user_id	INT	Идентификатор пользователя (если применимо)
session_id	UUID	Идентификатор сессии
metadata	JSONB	Дополнительные данные (например, текст ошибки, параметры операции)
