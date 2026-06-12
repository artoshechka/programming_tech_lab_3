/// @file parser_base.hpp
/// @brief Базовый класс парсеров с общей зависимостью-логгером
/// @author Artemenko Anton

#ifndef GUID_3a7f1d20_8c4e_4b6a_9f01_2d5e7c9a4b13
#define GUID_3a7f1d20_8c4e_4b6a_9f01_2d5e7c9a4b13

#include <logger/ilogger.hpp>
#include <memory>
#include <parser/iparser.hpp>
#include <utility>

namespace parser
{
/// @brief Базовая реализация парсера: хранит логгер, оставляя Load() абстрактным.
/// @details Выносит общую зависимость из чистого интерфейса IParser.
class ParserBase : public IParser
{
   public:
    /// @brief Конструктор базового парсера.
    /// @param[in] logger Логгер для записи диагностических сообщений.
    explicit ParserBase(const std::shared_ptr<logger::ILogger>& logger) : logger_(logger)
    {
    }

   protected:
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер.
};
}  // namespace parser
#endif  // GUID_3a7f1d20_8c4e_4b6a_9f01_2d5e7c9a4b13
