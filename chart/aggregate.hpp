#ifndef GUID_ee1a2b3c_4d5e_6f70_8192_a3b4c5d6e7f8
#define GUID_ee1a2b3c_4d5e_6f70_8192_a3b4c5d6e7f8

#include <data_model/src/timeline_data.hpp>
#include <map>
#include <string>
#include <vector>
#include <cmath>

namespace chart {

/// @brief Порог: если точек больше — агрегируем.
constexpr int kAggregateThreshold = 50;

/// @brief Длина ключа агрегации: 7 → "MM.YYYY", 4 → "YYYY".
constexpr int kAggKeyLen = 7;

/// @brief Агрегирует точки по префиксу метки времени, вычисляя среднее.
/// @details Формат "DD.MM.YYYY …" → offset=3; иначе offset=0 (ISO и прочее).
inline data::TimelineData Aggregate(const data::TimelineData& data, int keyLen = kAggKeyLen) {
    std::map<std::string, std::pair<double, int>> acc;
    std::vector<std::string> order;

    for (const auto& pt : data.points_) {
        const std::string& raw = pt.time_;
        const int offset = (raw.size() > 3 && raw[2] == '.') ? 3 : 0;
        const std::string key = raw.substr(offset, keyLen);
        if (acc.find(key) == acc.end()) order.push_back(key);
        acc[key].first  += pt.value_;
        acc[key].second += 1;
    }

    data::TimelineData result;
    result.name_ = data.name_;
    result.points_.reserve(order.size());
    for (const auto& k : order) {
        const auto& [sum, cnt] = acc.at(k);
        result.points_.push_back({k, std::round(sum / cnt * 100.0) / 100.0});
    }
    return result;
}

} // namespace chart

#endif // GUID_ee1a2b3c_4d5e_6f70_8192_a3b4c5d6e7f8
