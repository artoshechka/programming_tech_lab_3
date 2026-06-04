/// @file
/// @brief Реализация потокобезопасного базового логгера
/// @author Artemenko Anton

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread_safe_logger.hpp>

using logger::LogLevel;
using logger::ThreadSafeLogger;

ThreadSafeLogger::ThreadSafeLogger(const std::string& componentName, LogOutput output)
    : componentName_(componentName), settings_(std::string(), LogLevel::Debug, output)
{
}

ThreadSafeLogger::~ThreadSafeLogger()
{
    if (logFile_.is_open())
    {
        logFile_.close();
    }
}

void ThreadSafeLogger::SetSettings(const logger::LoggerSettings& settings)
{
    std::lock_guard<std::mutex> lock(syncMutex_);

    settings_ = settings;

    if (logFile_.is_open())
    {
        logFile_.close();
    }
    OpenLogFile();
}

void ThreadSafeLogger::OpenLogFile()
{
    if (settings_.output_ != LogOutput::File) return;
    if (!settings_.logFilePath_.has_value() || settings_.logFilePath_->empty()) return;

    const std::string& logFilePath = settings_.logFilePath_.value();
    const auto parent = std::filesystem::path(logFilePath).parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent);
    logFile_.open(logFilePath, std::ios::app);
}

logger::LoggerSettings ThreadSafeLogger::GetSettings() const
{
    std::lock_guard<std::mutex> lock(syncMutex_);
    return settings_;
}

std::string ThreadSafeLogger::LogLevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Trace:
            return "TRACE";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Fatal:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

void ThreadSafeLogger::Log(LogLevel level, const std::string& message, const char* file, int line, const char* function)
{
    std::lock_guard<std::mutex> lock(syncMutex_);

    if (level < settings_.logLevel_)
    {
        return;
    }

    const std::string logEntry = FormatMessage(level, message, file, line, function);

    if (settings_.output_ == LogOutput::Console)
    {
        std::cout << logEntry << '\n';
    } else if (settings_.output_ == LogOutput::File)
    {
        // Файл открывается в SetSettings(); если он не открыт — выводить некуда.
        if (logFile_.is_open())
        {
            logFile_ << logEntry << '\n';
        }
    }
}
