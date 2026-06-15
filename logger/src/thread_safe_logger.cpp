/// @file
/// @brief Реализация потокобезопасного базового логгера
/// @author Artemenko Anton

#include <QDir>
#include <QFileInfo>
#include <chrono>
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
    std::lock_guard<std::recursive_mutex> lock(syncMutex_);

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
    try
    {
        const QString parent = QFileInfo(QString::fromStdString(logFilePath)).absolutePath();
        if (!parent.isEmpty()) QDir().mkpath(parent);
        logFile_.open(logFilePath, std::ios::app);
        if (!logFile_.is_open())
        {
            throw std::ios_base::failure("ofstream::open returned closed stream");
        }
    } catch (const std::exception& ex)
    {
        // Отказ файлового вывода не должен ронять приложение и оставлять логгер
        // в "File без файла" состоянии — переключаемся на консоль с уведомлением.
        std::cerr << "[logger] Failed to open log file '" << logFilePath << "': " << ex.what()
                  << ". Falling back to console output." << std::endl;
        settings_.output_ = LogOutput::Console;
    }
}

logger::LoggerSettings ThreadSafeLogger::GetSettings() const
{
    std::lock_guard<std::recursive_mutex> lock(syncMutex_);
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
    std::lock_guard<std::recursive_mutex> lock(syncMutex_);

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
