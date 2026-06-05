/// @file
/// @brief Потокобезопасный базовый логгер
/// @author Artemenko Anton
#ifndef GUID_d1cbe2e9_7462_42f5_90e5_52ffccfe7677
#define GUID_d1cbe2e9_7462_42f5_90e5_52ffccfe7677

#include <fstream>
#include <ilogger.hpp>
#include <mutex>

namespace logger
{
/// @brief Потокобезопасная базовая реализация логгера
class ThreadSafeLogger : public ILogger
{
   public:
    /// @brief Конструктор потокобезопасного логгера
    /// @param[in] componentName Имя компонента для идентификации в логах
    /// @param[in] output Режим вывода логов
    explicit ThreadSafeLogger(const std::string& componentName, LogOutput output = LogOutput::Console);

    /// @brief Виртуальный деструктор
    ~ThreadSafeLogger() override;

    ThreadSafeLogger(const ThreadSafeLogger&) = delete;             ///< Копирование запрещено (mutex/файл).
    ThreadSafeLogger& operator=(const ThreadSafeLogger&) = delete;  ///< Присваивание копированием запрещено.
    ThreadSafeLogger(ThreadSafeLogger&&) = delete;                  ///< Перемещение запрещено (mutex).
    ThreadSafeLogger& operator=(ThreadSafeLogger&&) = delete;       ///< Присваивание перемещением запрещено.

    /// @brief Установить все настройки логгера одним вызовом
    /// @param[in] settings Набор настроек логгера
    void SetSettings(const LoggerSettings& settings) override;

    /// @brief Получить текущие настройки логгера
    /// @return Текущий набор настроек
    LoggerSettings GetSettings() const override;

    /// @brief Записать сообщение в лог с полной синхронизацией
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя исходного файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    void Log(LogLevel level, const std::string& message, const char* file = nullptr, int line = 0,
             const char* function = nullptr) override;

   protected:
    /// @brief Форматирует итоговую строку лога для конкретного типа логгера
    /// @details Переопределяется в производных классах для специфического формата сообщений
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя файла источника
    /// @param[in] line Номер строки источника
    /// @param[in] function Имя функции источника
    /// @return Отформатированное сообщение для вывода
    virtual std::string FormatMessage(LogLevel level, const std::string& message, const char* file, int line,
                                      const char* function) const = 0;

    /// @brief Преобразует уровень логирования в строку
    /// @param[in] level Уровень логирования
    /// @return Строковое представление уровня
    static std::string LogLevelToString(LogLevel level);

   protected:
    std::string componentName_;  ///< Имя компонента логирования для идентификации в формате сообщений

   private:
    /// @brief Открывает (или переоткрывает) файл лога согласно текущим настройкам.
    /// @details Вызывается под syncMutex_. При выводе не в файл или пустом пути файл остаётся закрытым.
    void OpenLogFile();

    std::ofstream logFile_;                   ///< Открытый файл логов для записи
    mutable std::recursive_mutex syncMutex_;  ///< Рекурсивный мьютекс: допускает вложенный Log() из callback'ов
                                              ///< внутри FormatMessage без дедлока.
    LoggerSettings settings_;                 ///< Текущие настройки логгера
};

}  // namespace logger

#endif  // GUID_d1cbe2e9_7462_42f5_90e5_52ffccfe7677