#ifndef TIMER_LOGGER_H
#define TIMER_LOGGER_H
#include <cstdint>
#include <string>

enum class LoggerLevel : uint8_t {
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4
};

class Logger {
    // 日志状态
    inline static auto level = LoggerLevel::INFO;

public:
    // 设置日志等级
    static void setLevel(LoggerLevel l) {
        level = l;
    }

    // info
    static void info(const std::string& s) {
        if (level <= LoggerLevel::INFO) printf("[INFO] %s\n", s.c_str());
    }

    // info
    static void info(const std::string& className, const std::string& s) {
        if (level <= LoggerLevel::INFO) printf("[INFO] [%s] %s\n", className.c_str(), s.c_str());
    }

    // debug
    static void debug(const std::string& s) {
        if (level <= LoggerLevel::DEBUG) printf("[DEBUG] %s\n", s.c_str());
    }

    // debug
    static void debug(const std::string& className, const std::string& s) {
        if (level <= LoggerLevel::DEBUG) printf("[DEBUG] [%s] %s\n", className.c_str(), s.c_str());
    }

    // warn
    static void warn(const std::string& s) {
        if (level <= LoggerLevel::WARN) printf("[WARN] %s\n", s.c_str());
    }

    // warn
    static void warn(const std::string& className, const std::string& s) {
        if (level <= LoggerLevel::WARN) printf("[WARN] [%s] %s\n", className.c_str(), s.c_str());
    }

    // error
    static void error(const std::string& s) {
        if (level <= LoggerLevel::ERROR) printf("[ERROR] %s\n", s.c_str());
    }

    // error
    static void error(const std::string& className, const std::string& s) {
        if (level <= LoggerLevel::ERROR) printf("[ERROR] [%s] %s\n", className.c_str(), s.c_str());
    }
};

#endif //TIMER_LOGGER_H