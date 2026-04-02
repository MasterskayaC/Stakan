#include "logger.h"

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace server;

void Logger::Log(const LogLevel level, const std::string& msg) {
    std::cout << std::format("[{}] [{}] {}\n", Now(), ToString(level), msg);
}

std::string Logger::Now() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

const char* Logger::ToString(const LogLevel level) {
    switch (level) {
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
    }
    return "";
}
