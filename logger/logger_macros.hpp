/// @file logger_macros.hpp
/// @brief Макросы для логирования
/// @author Artemenko Anton
#ifndef GUID_13d828c9_913c_478d_aab2_8f1404db6d7f
#define GUID_13d828c9_913c_478d_aab2_8f1404db6d7f

#include <src/log_entry_stream.hpp>

namespace logger
{
/// @brief Логирование на уровне Trace; прикрепляет файл/строку/функцию.
/// @param[in] loggerPtr Указатель на логгер
#define LogTrace(loggerPtr) \
    logger::detail::LogEntryStream((loggerPtr), logger::LogLevel::Trace, __FILE__, __LINE__, __FUNCTION__)

/// @brief Логирование на уровне Debug; прикрепляет файл/строку/функцию.
/// @param[in] loggerPtr Указатель на логгер
#define LogDebug(loggerPtr) \
    logger::detail::LogEntryStream((loggerPtr), logger::LogLevel::Debug, __FILE__, __LINE__, __FUNCTION__)

/// @brief Логирование на уровне Info; прикрепляет файл/строку/функцию.
/// @param[in] loggerPtr Указатель на логгер
#define LogInfo(loggerPtr) \
    logger::detail::LogEntryStream((loggerPtr), logger::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__)

/// @brief Логирование на уровне Warning; прикрепляет файл/строку/функцию.
/// @param[in] loggerPtr Указатель на логгер
#define LogWarning(loggerPtr) \
    logger::detail::LogEntryStream((loggerPtr), logger::LogLevel::Warning, __FILE__, __LINE__, __FUNCTION__)

/// @brief Логирование на уровне Error; прикрепляет файл/строку/функцию.
/// @param[in] loggerPtr Указатель на логгер
#define LogError(loggerPtr) \
    logger::detail::LogEntryStream((loggerPtr), logger::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__)

/// @brief Логирование на уровне Fatal; прикрепляет файл/строку/функцию.
/// @param[in] loggerPtr Указатель на логгер
#define LogFatal(loggerPtr) \
    logger::detail::LogEntryStream((loggerPtr), logger::LogLevel::Fatal, __FILE__, __LINE__, __FUNCTION__)
}  // namespace logger
#endif  // GUID_13d828c9_913c_478d_aab2_8f1404db6d7f