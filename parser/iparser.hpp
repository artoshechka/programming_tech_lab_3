/// @file iparser.hpp
/// @brief Интерфейс парсера
/// @author Artemenko Anton

#ifndef GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
#define GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5

#include <data_model/src/timeline_data.hpp>
#include <string>

namespace parser
{
/// @brief Интерфейс для парсера данных временного ряда.
class IParser
{
   public:
    virtual ~IParser() = default;

    /// @brief Загрузить временной ряд из источника.
    /// @param[in] source Путь к источнику данных (файл БД или JSON).
    /// @return Загруженный временной ряд.
    virtual data::TimelineData Load(const std::string& source) = 0;

    /// @brief Сохранить временной ряд в приёмник.
    /// @param[in] data Временной ряд для сохранения.
    /// @param[in] destination Путь к приёмнику данных.
    virtual void Save(const data::TimelineData& data, const std::string& destination) = 0;
};
}  // namespace parser
#endif  // GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
