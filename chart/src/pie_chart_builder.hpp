/// @file pie_chart_builder.hpp
/// @brief Объявление построителя круговой диаграммы.
/// @author Artemenko Anton
#ifndef GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
#define GUID_c3d4e5f6_a7b8_9012_cdef_123456789012

#include <chart/ichart_builder.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <utility>

namespace chart
{

/// @brief Построитель круговой диаграммы.
class PieChartBuilder : public IChartBuilder
{
   public:
    /// @brief Конструктор построителя.
    /// @param[in] logger Логгер для диагностики построения; допускается nullptr.
    explicit PieChartBuilder(std::shared_ptr<logger::ILogger> logger = nullptr) : logger_(std::move(logger)) {}

    /// @brief Применяет опции построения (использует BuilderOptions::aggregate и palette).
    /// @param[in] options Опции; aggregate=true включает группировку при превышении порога,
    ///                    palette — необязательная палитра для покраски срезов.
    void Configure(const BuilderOptions& options) noexcept override
    {
        aggregate_ = options.aggregate;
        palette_ = options.palette;
    }

    /// @brief Строит круговую диаграмму на основе данных.
    /// @details При включённом флаге агрегации точки группируются по префиксу ключа
    ///          (при превышении порога), затем оставляются до 10 крупнейших срезов;
    ///          остаток сливается в сектор "Other". При выключенном флаге показываются
    ///          все срезы без агрегации и без "Other". Метки выводятся снаружи.
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] std::unique_ptr<QtCharts::QChart> Build(const data::TimelineData& data) override;

   private:
    bool aggregate_ = false;                   ///< Флаг включения агрегации входных точек.
    const style::IPalette* palette_ = nullptr; ///< Палитра для покраски срезов (опционально).
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace chart

#endif  // GUID_c3d4e5f6_a7b8_9012_cdef_123456789012
