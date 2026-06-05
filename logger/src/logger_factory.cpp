/// @file
/// @brief Определение фабричных функций singleton-логгеров
/// @author Artemenko Anton

#include <app_logger.hpp>
#include <logger_factory.hpp>

namespace logger
{
namespace
{
template <typename TLoggerImpl>
std::shared_ptr<ILogger> CreateAndConfigureLogger(const std::string& logPath)
{
    auto instance = std::make_shared<TLoggerImpl>();
    instance->SetSettings(LoggerSettings(logPath, LogLevel::Debug, LogOutput::Console));
    return instance;
}

std::shared_ptr<ILogger> CreateAppLogger()
{
    return CreateAndConfigureLogger<AppLogger>("logs/app.log");
}
}  // namespace

template <>
std::shared_ptr<ILogger> GetLogger<AppLoggerTag>()
{
    // C++11 гарантирует потокобезопасную инициализацию static local — гонка между
    // потоками при первом вызове GetLogger<> невозможна.
    static const std::shared_ptr<ILogger> kLogger = CreateAppLogger();
    return kLogger;
}

}  // namespace logger
