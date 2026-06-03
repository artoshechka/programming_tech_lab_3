/// @file bar_chart_builder.hpp
/// @brief Объявление построителя столбчатой диаграммы.
/// @author Artemenko Anton
#ifndef GUID_d4e5f6a7_b8c9_0123_defa_234567890123
#define GUID_d4e5f6a7_b8c9_0123_defa_234567890123

#include <chart/ichart_builder.hpp>

namespace chart {

/// @brief Построитель столбчатой диаграммы.
class BarChartBuilder : public IChartBuilder {
public:
    /// @brief Строит столбчатую диаграмму на основе данных.
    /// @details При числе точек выше порога агрегации данные предварительно
    ///          усредняются; ось значений масштабируется с отступом 15%.
    /// @param[in] data Входные данные временного ряда.
    /// @return Указатель на построенный QChart (владение передаётся вызывающему).
    QChart* Build(const data::TimelineData& data) override;
};

} // namespace chart

#endif // GUID_d4e5f6a7_b8c9_0123_defa_234567890123
