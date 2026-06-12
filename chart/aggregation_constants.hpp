/// @file aggregation_constants.hpp
/// @brief Константы агрегации точек временного ряда.
/// @author Artemenko Anton
#ifndef GUID_b2c3d4e5_f607_4891_b2c3_d4e5f6071829
#define GUID_b2c3d4e5_f607_4891_b2c3_d4e5f6071829

namespace chart
{

constexpr int kAggregateThreshold = 50;  ///< Порог числа точек: при превышении применяется агрегация.
constexpr int kAggKeyLen = 7;            ///< Длина ключа агрегации по умолчанию: 7 → "MM.YYYY", 4 → "YYYY".
constexpr int kDayPrefixLen = 3;         ///< Длина префикса "DD." в формате "DD.MM.YYYY".
constexpr double kRoundFactor = 100.0;   ///< Множитель округления среднего до двух знаков.

}  // namespace chart

#endif  // GUID_b2c3d4e5_f607_4891_b2c3_d4e5f6071829
