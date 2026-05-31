#ifndef GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
#define GUID_c3d4e5f6_a7b8_9012_cdef_123456789012

#include <chart/ichart_builder.hpp>

namespace chart {

/// @brief Построитель круговой диаграммы.
class PieChartBuilder : public IChartBuilder {
public:
    QChart* Build(const data::TimelineData& data) override;
};

} // namespace chart

#endif // GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
