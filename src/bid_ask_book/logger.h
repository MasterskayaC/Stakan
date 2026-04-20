#pragma once

#include <string>

namespace server {
    enum class LogLevel {
        Info,
        Warning,
        Error
    };

    class Logger {
    public:
        static void Log(LogLevel level, const std::string& msg);

    private:
        static std::string Now();
        static const char* ToString(LogLevel level);
    };
}
