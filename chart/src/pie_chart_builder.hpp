/// @file pie_chart_builder.hpp
/// @brief Объявление построителя круговой диаграммы.
/// @author Artemenko Anton
#ifndef GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
#define GUID_c3d4e5f6_a7b8_9012_cdef_123456789012

#include <chart/ichart_builder.hpp>

namespace chart
{

/// @brief Построитель круговой диаграммы.
class PieChartBuilder : public IChartBuilder
{
   public:
    /// @brief Строит круговую диаграмму на основе данных.
    /// @details При включённом флаге агрегации точки группируются по префиксу ключа
    ///          (при превышении порога), затем оставляются до 10 крупнейших срезов;
    ///          остаток сливается в сектор "Other". При выключенном флаге показываются
    ///          все срезы без агрегации и без "Other". Метки выводятся снаружи.
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] std::unique_ptr<QChart> Build(const data::TimelineData& data) override;

    /// @brief Включает или отключает агрегацию входных точек.
    /// @param[in] enabled true — агрегировать при превышении порога; false — оставлять как есть.
    void SetAggregate(bool enabled) noexcept { aggregate_ = enabled; }

   private:
    bool aggregate_ = false;  ///< Флаг включения агрегации входных точек.
};

}  // namespace chart

#endif  // GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
