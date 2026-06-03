/// @file pie_chart_builder.hpp
/// @brief Объявление построителя круговой диаграммы.
/// @author Artemenko Anton
#ifndef GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
#define GUID_c3d4e5f6_a7b8_9012_cdef_123456789012

#include <chart/ichart_builder.hpp>

namespace chart {

/// @brief Построитель круговой диаграммы.
class PieChartBuilder : public IChartBuilder {
public:
    /// @brief Строит круговую диаграмму на основе данных.
    /// @details При числе точек выше порога агрегации данные предварительно
    ///          усредняются. Отображаются до 10 крупнейших срезов; остаток
    ///          объединяется в сектор "Other". Метки выводятся снаружи.
    /// @param[in] data Входные данные временного ряда.
    /// @return Указатель на построенный QChart (владение передаётся вызывающему).
    QChart* Build(const data::TimelineData& data) override;
};

} // namespace chart

#endif // GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
