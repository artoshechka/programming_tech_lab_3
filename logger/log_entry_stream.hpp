/// @file
/// @brief Объявление потокового helper'а для макросов логирования
/// @author Artemenko Anton
#ifndef GUID_8b9db8a0_64a9_4a28_a8e2_ef99f9a87516
#define GUID_8b9db8a0_64a9_4a28_a8e2_ef99f9a87516

#include <ilogger.hpp>
#include <memory>
#include <sstream>
#include <string>

namespace logger
{
namespace detail
{
/// @brief Вспомогательный потоковый объект для макросов вида LogDebug(ptr) << "message"
class LogEntryStream final
{
   public:
    /// @brief Конструктор потокового сообщения
    /// @param[in] logger Умный указатель на целевой логгер
    /// @param[in] level Уровень логирования
    /// @param[in] file Имя файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    LogEntryStream(const std::shared_ptr<ILogger>& logger, LogLevel level, const char* file, int line, const char* function);

    /// @brief Деструктор, автоматически отправляет собранное сообщение в логгер
    ~LogEntryStream();

    LogEntryStream(const LogEntryStream&) = delete;             ///< Копирование запрещено (RAII-отправка в dtor).
    LogEntryStream& operator=(const LogEntryStream&) = delete;  ///< Присваивание копированием запрещено.
    LogEntryStream(LogEntryStream&&) = delete;                  ///< Перемещение запрещено.
    LogEntryStream& operator=(LogEntryStream&&) = delete;       ///< Присваивание перемещением запрещено.

    template <typename T>
    LogEntryStream& operator<<(const T& value)
    {
        stream_ << value;
        return *this;
    }

   private:
    std::shared_ptr<ILogger> logger_;  ///< Целевой логгер.
    LogLevel level_;                   ///< Уровень логирования сообщения
    const char* file_;                 ///< Имя файла источника
    int line_;                         ///< Номер строки источника
    const char* function_;             ///< Имя функции источника
    std::ostringstream stream_;        ///< Буфер накопления текста сообщения.
};

}  // namespace detail
}  // namespace logger

#endif  // GUID_8b9db8a0_64a9_4a28_a8e2_ef99f9a87516
