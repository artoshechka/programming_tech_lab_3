/// @file iparser_registry.hpp
/// @brief Интерфейс реестра парсеров
/// @author Artemenko Anton

#ifndef GUID_a7b8c9d0_e1f2_3456_abcd_567890123456
#define GUID_a7b8c9d0_e1f2_3456_abcd_567890123456

#include <memory>
#include <string>
#include <parser/iparser.hpp>

namespace parser {

/// @brief Реестр парсеров с диспетчеризацией по расширению файла.
class IParserRegistry {
public:
    /// @brief Виртуальный деструктор.
    virtual ~IParserRegistry() = default;

    /// @brief Возвращает парсер для указанного расширения.
    /// @param[in] extension Расширение файла без точки (например, "json", "sqlite").
    /// @return Парсер или nullptr, если расширение не зарегистрировано.
    virtual std::shared_ptr<IParser> Get(const std::string& extension) const = 0;
};

} // namespace parser

#endif // GUID_a7b8c9d0_e1f2_3456_abcd_567890123456
