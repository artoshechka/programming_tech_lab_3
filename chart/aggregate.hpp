/// @file aggregate.hpp
/// @brief Утилита агрегации точек временного ряда по префиксу метки времени.
/// @author Artemenko Anton
#ifndef GUID_ee1a2b3c_4d5e_6f70_8192_a3b4c5d6e7f8
#define GUID_ee1a2b3c_4d5e_6f70_8192_a3b4c5d6e7f8

#include <chart/aggregation_constants.hpp>
#include <cmath>
#include <data_model/timeline_data.hpp>
#include <map>
#include <string>
#include <vector>

namespace chart
{

/// @brief Агрегирует точки временного ряда по префиксу метки времени, вычисляя среднее.
/// @details Если метка имеет формат "DD.MM.YYYY …" (третий символ — точка),
///          используется смещение 3 для пропуска дня; иначе смещение 0 (ISO 8601 и прочее).
///          Значения в каждой группе усредняются и округляются до двух знаков после запятой.
/// @param[in] data   Исходный временной ряд.
/// @param[in] keyLen Длина ключа агрегации (по умолчанию kAggKeyLen).
/// @return Новый временной ряд с агрегированными точками.
inline data::TimelineData Aggregate(const data::TimelineData& data, int keyLen = kAggKeyLen)
{
    std::map<std::string, std::pair<double, int>> acc;
    std::vector<std::string> order;

    for (const auto& pt : data.points_)
    {
        const std::string& raw = pt.time_;
        const int offset = (raw.size() > kDayPrefixLen && raw[2] == '.') ? kDayPrefixLen : 0;
        const std::string key = raw.substr(offset, keyLen);
        const auto [it, inserted] = acc.try_emplace(key, 0.0, 0);
        if (inserted) order.push_back(key);
        it->second.first += pt.value_;
        it->second.second += 1;
    }

    data::TimelineData result;
    result.name_ = data.name_;
    result.points_.reserve(order.size());
    for (const auto& k : order)
    {
        const auto& [sum, cnt] = acc.at(k);
        result.points_.push_back({k, std::round(sum / cnt * kRoundFactor) / kRoundFactor});
    }
    return result;
}

}  // namespace chart

#endif  // GUID_ee1a2b3c_4d5e_6f70_8192_a3b4c5d6e7f8
