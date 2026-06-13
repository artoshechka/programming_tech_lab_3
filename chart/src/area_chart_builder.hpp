/// @file area_chart_builder.hpp
/// @brief Объявление построителя диаграммы с областями (area) по реальной оси времени.
/// @author Artemenko Anton
#ifndef GUID_9c4d1e72_8b30_4f6a_a2d5_6e1b73f08c94
#define GUID_9c4d1e72_8b30_4f6a_a2d5_6e1b73f08c94

#include <chart/ichart_builder.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <utility>

namespace chart
{

/// @brief Построитель диаграммы с областями: value(t) с заливкой под кривой.
/// @details Как и линия времени, не агрегирует данные и кладёт точки на реальную ось
///          QDateTimeAxis, но заливает площадь под кривой — подчёркивает форму, объём и
///          динамику временного ряда.
class AreaChartBuilder : public IChartBuilder
{
   public:
    /// @brief Конструктор построителя.
    /// @param[in] logger Логгер для диагностики построения; допускается nullptr.
    explicit AreaChartBuilder(const std::shared_ptr<logger::ILogger>& logger = nullptr) : logger_(logger)
    {
    }

    /// @brief Применяет опции построения (использует только palette).
    /// @param[in] options Опции; palette — необязательная палитра для покраски области.
    /// @details Флаг агрегации игнорируется: диаграмма всегда показывает все точки.
    void Configure(const BuilderOptions& options) noexcept override
    {
        palette_ = options.palette;
    }

    /// @brief Строит диаграмму с областями на основе данных.
    /// @details Метки времени парсятся как "dd.MM.yyyy HH:mm" (с фолбэком на "dd.MM.yyyy"
    ///          и ISO 8601) и кладутся на ось QDateTimeAxis. Если ни одну метку распознать
    ///          не удалось, X становится порядковым индексом точки (QValueAxis).
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] std::unique_ptr<QtCharts::QChart> Build(const data::TimelineData& data) override;

   private:
    const style::IPalette* palette_ = nullptr;  ///< Палитра для покраски области (опционально).
    std::shared_ptr<logger::ILogger> logger_;   ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace chart

#endif  // GUID_9c4d1e72_8b30_4f6a_a2d5_6e1b73f08c94
