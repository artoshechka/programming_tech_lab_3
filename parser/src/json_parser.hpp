/// @file json_parser.hpp
/// @brief Объявление парсера JSON
/// @author Artemenko Anton

#ifndef GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0
#define GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0

#include <parser/iparser.hpp>
namespace parser
{
/// @brief Парсер для обработки JSON данных.
class JsonParser : public IParser

{
    ~JsonParser() = default;
};

}  // namespace parser
#endif  // GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0