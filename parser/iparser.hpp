/// @file iparser.hpp
/// @brief Интерфейс парсера
/// @author Artemenko Anton

#ifndef GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
#define GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5

namespace parser
{
/// @brief Интерфейс для парсера данных.
class IParser
{
   public:
    virtual ~IParser() = default;

    /// @brief Метод для парсинга данных.
    virtual void Parse() = 0;
};
}  // namespace parser
#endif  // GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
