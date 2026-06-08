/// @file bar_chart_builder.hpp
/// @brief Объявление построителя столбчатой диаграммы.
/// @author Artemenko Anton
#ifndef GUID_d4e5f6a7_b8c9_0123_defa_234567890123
#define GUID_d4e5f6a7_b8c9_0123_defa_234567890123

#include <chart/ichart_builder.hpp>

namespace chart
{

/// @brief Построитель столбчатой диаграммы.
class BarChartBuilder : public IChartBuilder
{
   public:
    /// @brief Применяет опции построения (использует только palette).
    /// @param[in] options Опции; palette — необязательная палитра для покраски наборов.
    void Configure(const BuilderOptions& options) noexcept override { palette_ = options.palette; }

    /// @brief Строит столбчатую диаграмму на основе данных.
    /// @details При числе точек выше порога агрегации данные предварительно
    ///          усредняются; ось значений масштабируется с отступом 15%.
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] std::unique_ptr<QtCharts::QChart> Build(const data::TimelineData& data) override;

   private:
    const style::IPalette* palette_ = nullptr; ///< Палитра для покраски барсетов (опционально).
};

}  // namespace chart

#endif  // GUID_d4e5f6a7_b8c9_0123_defa_234567890123
