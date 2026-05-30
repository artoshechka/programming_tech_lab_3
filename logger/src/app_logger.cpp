/// @file
/// @brief Определение логгера приложения
/// @author Artemenko Anton

#include <app_logger.hpp>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

using logger::AppLogger;
using logger::LogLevel;

AppLogger::AppLogger(LogOutput output) : ThreadSafeLogger("APP", output)
{
}

AppLogger::~AppLogger() = default;

static std::string currentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::tm tm_buf{};
    localtime_r(&t, &tm_buf);
    std::ostringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string AppLogger::FormatMessage(LogLevel level, const std::string& message, const char* file, int line,
                                     const char* function) const
{
    std::string fileInfo;
    if (file != nullptr)
    {
        fileInfo = std::filesystem::path(file).filename().string() + ':' + std::to_string(line);
    }

    std::string functionInfo = function ? function : "";

    return '[' + currentTimestamp() + "] [" + componentName_ + "] [" + ThreadSafeLogger::LogLevelToString(level) +
           "] " + fileInfo + " - " + functionInfo + " - " + message;
}
