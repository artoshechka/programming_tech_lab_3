/// @file parser_limits.hpp
/// @brief Лимиты парсеров (приватные детали реализации).
/// @author Artemenko Anton
#ifndef GUID_d4e5f607_1829_4a13_d4e5_f60718293a4b
#define GUID_d4e5f607_1829_4a13_d4e5_f60718293a4b

#include <QtGlobal>

namespace parser
{

constexpr qint64 kMaxJsonBytes = 256LL * 1024 * 1024;  ///< Максимальный размер JSON-файла (256 МиБ).

}  // namespace parser

#endif  // GUID_d4e5f607_1829_4a13_d4e5_f60718293a4b
