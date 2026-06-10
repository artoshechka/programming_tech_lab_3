/// @file parser_registry.hpp
/// @brief Объявление реестра парсеров
/// @author Artemenko Anton

#ifndef GUID_b8c9d0e1_f2a3_4567_bcde_678901234567
#define GUID_b8c9d0e1_f2a3_4567_bcde_678901234567

#include <logger/ilogger.hpp>
#include <map>
#include <memory>
#include <parser/iparser_registry.hpp>
#include <utility>

namespace parser
{

/// @brief Реестр парсеров: extension → IParser.
class ParserRegistry : public IParserRegistry
{
   public:
    /// @brief Конструктор реестра.
    /// @param[in] logger Логгер для диагностики регистрации/резолва; допускается nullptr.
    explicit ParserRegistry(const std::shared_ptr<logger::ILogger>& logger = nullptr) : logger_(logger)
    {
    }

    /// @brief Регистрирует парсер для указанного расширения.
    /// @param[in] extension Расширение файла (регистр игнорируется, приводится к нижнему).
    /// @param[in] parser Парсер, связываемый с расширением.
    void Register(const std::string& extension, std::shared_ptr<IParser> parser);

    /// @brief Возвращает парсер для указанного расширения.
    /// @param[in] extension Расширение файла без точки (например, "json", "sqlite").
    /// @return Парсер или nullptr, если расширение не зарегистрировано.
    std::shared_ptr<IParser> Get(const std::string& extension) const override;

    /// @brief Возвращает все зарегистрированные расширения.
    /// @return Расширения в нижнем регистре, без точки.
    std::vector<std::string> SupportedExtensions() const override;

   private:
    std::map<std::string, std::shared_ptr<IParser>> parsers_;  ///< Соответствие расширение → парсер.
    std::shared_ptr<logger::ILogger> logger_;                  ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace parser

#endif  // GUID_b8c9d0e1_f2a3_4567_bcde_678901234567
