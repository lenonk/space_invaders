#pragma once

#include <iostream>
#include <ostream>
#include <string>

namespace SpaceInvaders {

enum class LogLevel { Debug, Info, Warning, Error };

class Logger {
public:
    static void Log(const LogLevel level, const std::string &message) {
        std::string error;
        switch (level) {
            case LogLevel::Debug:
                error = "[DEBUG] ";
                break;
            case LogLevel::Info:
                error = "[INFO] ";
                break;
            case LogLevel::Warning:
                error = "[WARNING] ";
                break;
            default:
                error = "[UNKNOWN] ";
                break;
        }

        std::println(std::cerr, "{}: {}", error, message);
    }
};

#define LogDebug(msg) Logger::Log(LogLevel::Debug, msg)
#define LogInfo(msg) Logger::Log(LogLevel::Info, msg)
#define LogWarning(msg) Logger::Log(LogLevel::Warning, msg)
#define LogError(msg) Logger::Log(LogLevel::Error, msg)

}