/// @file line_chart_builder.hpp
/// @brief Объявление построителя линии времени (настоящий таймлайн).
/// @author Artemenko Anton
#ifndef GUID_3f1c8a52_7d94_4e60_b1a8_92c4ef07d5a3
#define GUID_3f1c8a52_7d94_4e60_b1a8_92c4ef07d5a3

#include <chart/ichart_builder.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <utility>

namespace chart
{

/// @brief Построитель линии времени: value(t) на реальной оси времени.
/// @details В отличие от Bar/Pie не агрегирует данные — рисует все точки в
///          хронологическом порядке, сохраняя суть временного ряда.
class LineChartBuilder : public IChartBuilder
{
   public:
    /// @brief Конструктор построителя.
    /// @param[in] logger Логгер для диагностики построения; допускается nullptr.
    explicit LineChartBuilder(const std::shared_ptr<logger::ILogger>& logger = nullptr) : logger_(logger)
    {
    }

    /// @brief Применяет опции построения (использует только palette).
    /// @param[in] options Опции; palette — необязательная палитра для покраски линии.
    /// @details Флаг агрегации игнорируется: линия времени всегда показывает все точки.
    void Configure(const BuilderOptions& options) noexcept override
    {
        palette_ = options.palette;
    }

    /// @brief Строит линию времени на основе данных.
    /// @details Метки времени парсятся как "dd.MM.yyyy HH:mm" (с фолбэком на "dd.MM.yyyy"
    ///          и ISO 8601) и кладутся на ось QDateTimeAxis. Если ни одну метку распознать
    ///          не удалось, X становится порядковым индексом точки (QValueAxis).
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] std::unique_ptr<QtCharts::QChart> Build(const data::TimelineData& data) override;

   private:
    const style::IPalette* palette_ = nullptr;  ///< Палитра для покраски линии (опционально).
    std::shared_ptr<logger::ILogger> logger_;   ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace chart

#endif  // GUID_3f1c8a52_7d94_4e60_b1a8_92c4ef07d5a3
