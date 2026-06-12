/// @file timeline_data.hpp
/// @brief Объявление класса TimelineData и структуры TimePoint
/// @author Artemenko Anton
#ifndef GUID_f0ad56a1_3baa_4836_a15e_b8803adf069b
#define GUID_f0ad56a1_3baa_4836_a15e_b8803adf069b

#include <data_model/schema.hpp>
#include <string>
#include <utility>
#include <vector>

namespace data
{
/// @brief Одна точка временного ряда: метка времени и значение.
/// @details Хранит метку времени сырой строкой (формат БД/JSON), парсинг в дату — отдельной задачей.
struct TimePoint
{
    std::string time_;    ///< Сырая метка времени в строковом виде (например "01.01.2005 01:00")
    double value_ = 0.0;  ///< Числовое значение отсчёта

    /// @brief Статическая схема сериализации: поле -> строковое имя (совпадает с колонками БД)
    static constexpr auto kSchema =
        std::make_tuple(std::make_pair(&TimePoint::time_, "Time"), std::make_pair(&TimePoint::value_, "Value"));
};

/// @brief Временной ряд: имя и набор точек.
/// @details Владеет вектором TimePoint. Служит основной единицей обмена между парсером и GUI.
class TimelineData
{
   public:
    std::string name_;               ///< Имя ряда (совпадает с именем таблицы БД)
    std::vector<TimePoint> points_;  ///< Точки временного ряда

    /// @brief Статическая схема сериализации: поле -> строковое имя ключа JSON
    static constexpr auto kSchema =
        std::make_tuple(std::make_pair(&TimelineData::name_, "name"), std::make_pair(&TimelineData::points_, "points"));
};

}  // namespace data
#endif  // GUID_f0ad56a1_3baa_4836_a15e_b8803adf069b
