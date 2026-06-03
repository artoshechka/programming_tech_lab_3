/// @file parse_exception.hpp
/// @brief Исключение парсера
/// @author Artemenko Anton
#ifndef GUID_a1f2e3d4_b5c6_7890_abcd_ef1234567890
#define GUID_a1f2e3d4_b5c6_7890_abcd_ef1234567890

#include <stdexcept>

namespace parser
{
/// @brief Выбрасывается при ошибке разбора или открытия источника данных.
class ParseException : public std::runtime_error
{
   public:
    /// @brief Наследует конструкторы std::runtime_error (приём текста сообщения об ошибке).
    using std::runtime_error::runtime_error;
};
}  // namespace parser

#endif  // GUID_a1f2e3d4_b5c6_7890_abcd_ef1234567890
