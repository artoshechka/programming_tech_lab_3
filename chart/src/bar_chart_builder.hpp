/// @file bar_chart_builder.hpp
/// @brief Объявление построителя столбчатой диаграммы.
/// @author Artemenko Anton
#ifndef GUID_d4e5f6a7_b8c9_0123_defa_234567890123
#define GUID_d4e5f6a7_b8c9_0123_defa_234567890123

#include <chart/ichart_builder.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <utility>

namespace chart
{

/// @brief Построитель столбчатой диаграммы.
class BarChartBuilder : public IChartBuilder
{
   public:
    /// @brief Конструктор построителя.
    /// @param[in] logger Логгер для диагностики построения; допускается nullptr.
    explicit BarChartBuilder(std::shared_ptr<logger::ILogger> logger = nullptr) : logger_(std::move(logger)) {}

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
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace chart

#endif  // GUID_d4e5f6a7_b8c9_0123_defa_234567890123
