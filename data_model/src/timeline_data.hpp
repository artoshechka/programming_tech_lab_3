/// @file timeline_data.hpp
/// @brief Объявление класса TimelineData
/// @author Artemenko Anton
#ifndef GUID_f0ad56a1_3baa_4836_a15e_b8803adf069b
#define GUID_f0ad56a1_3baa_4836_a15e_b8803adf069b
#include <data_model/idata.hpp>
namespace data
{
/// @brief Класс для представления данных временной шкалы.
class TimelineData : public IData
{
   public:
    ~TimelineData() = default;
    /// @brief Метод для получения данных временной шкалы.
    void GetData() override;
};
}  // namespace data
#endif  // GUID_f0ad56a1_3baa_4836_a15e_b8803adf069b