класс GraphicalUI будет принимать указатель на объект client lib в (дальнейшем Объект) в конструкторе, запускать QAplication через статический метод run(int argc, char* argv[]). Взаимодестви с client lib строится на колбэк функциях, список сигнатур колбэк функций приведен ниже
        std::function<void()> on_connected;
        std::function<void()> on_disconnected;
        std::function<void(ConnectionState)> on_state_changed;
        std::function<void(const Snapshot&)> on_snapshot;
        std::function<void(const TopLevel&)> on_top_of_book;
        std::function<void(ClientError, std::string_view)> on_error;
