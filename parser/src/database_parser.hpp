/// @file database_parser.hpp
/// @brief Объявление парсера базы данных
/// @author Artemenko Anton

#ifndef GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb
#define GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb

#include <parser/iparser.hpp>

namespace parser
{
/// @brief Парсер для обработки данных базы данных.
class DatabaseParser : public IParser

{
    ~DatabaseParser() = default;
};

}  // namespace parser
#endif  // GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb