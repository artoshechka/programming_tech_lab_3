/// @file gantt_chart_builder.hpp
/// @brief Объявление построителя диаграммы-таймлайна (Gantt-стиль).
/// @author Artemenko Anton
#ifndef GUID_7b2e9d10_4af3_4c81_9e6d_1a5b8c3f02e7
#define GUID_7b2e9d10_4af3_4c81_9e6d_1a5b8c3f02e7

#include <chart/ichart_builder.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <utility>

namespace chart
{

/// @brief Построитель диаграммы-таймлайна: периоды как ступенчатые горизонтальные полосы.
/// @details Ряд агрегируется по месяцу; каждый период — отдельная горизонтальная полоса,
///          смещённая по реальной оси времени (ступеньками). Реализовано через
///          QHorizontalStackedBarSeries: прозрачный сегмент-смещение задаёт позицию полосы
///          во времени, цветной сегмент — её длину (длительность периода).
class GanttChartBuilder : public IChartBuilder
{
   public:
    /// @brief Конструктор построителя.
    /// @param[in] logger Логгер для диагностики построения; допускается nullptr.
    explicit GanttChartBuilder(const std::shared_ptr<logger::ILogger>& logger = nullptr) : logger_(logger)
    {
    }

    /// @brief Применяет опции построения (использует только palette).
    /// @param[in] options Опции; palette — необязательная палитра для покраски полос.
    /// @details Флаг агрегации игнорируется: таймлайн всегда группирует точки по месяцу.
    void Configure(const BuilderOptions& options) noexcept override
    {
        palette_ = options.palette;
    }

    /// @brief Строит диаграмму-таймлайн на основе данных.
    /// @details Точки агрегируются по месяцу (ключ "MM.YYYY"/"YYYY-MM"); каждый период
    ///          становится горизонтальной полосой на оси QDateTimeAxis, смещённой к своей дате.
    ///          Если метки периодов не распознаются как даты, используется порядковая ось.
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] std::unique_ptr<QtCharts::QChart> Build(const data::TimelineData& data) override;

   private:
    const style::IPalette* palette_ = nullptr;  ///< Палитра для покраски полос (опционально).
    std::shared_ptr<logger::ILogger> logger_;   ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace chart

#endif  // GUID_7b2e9d10_4af3_4c81_9e6d_1a5b8c3f02e7
