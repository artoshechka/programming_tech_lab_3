#ifndef GUID_d4e5f6a7_b8c9_0123_defa_234567890123
#define GUID_d4e5f6a7_b8c9_0123_defa_234567890123

#include <chart/ichart_builder.hpp>

namespace chart {

/// @brief Построитель столбчатой диаграммы.
class BarChartBuilder : public IChartBuilder {
public:
    QChart* Build(const data::TimelineData& data) override;
};

} // namespace chart

#endif // GUID_d4e5f6a7_b8c9_0123_defa_234567890123
