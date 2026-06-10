/// @file ichart_builder.hpp
/// @brief Объявление интерфейса построителя графика из временного ряда.
/// @author Artemenko Anton
#ifndef GUID_a1b2c3d4_e5f6_7890_abcd_ef1234567890
#define GUID_a1b2c3d4_e5f6_7890_abcd_ef1234567890

#include <data_model/src/timeline_data.hpp>
#include <memory>

// Опережающее объявление: интерфейс не тянет тяжёлый <QtCharts/QChart> в потребителей,
// которые видят лишь сигнатуры; полный тип нужен только реализациям (в .cpp) и тем,
// кто фактически уничтожает unique_ptr<QChart> (см. правило ptl3-qt-scope).
namespace QtCharts
{
class QChart;
}

namespace style
{
class IPalette;
}

namespace chart
{

/// @brief Опции построения графика, общие для всех типов.
/// @details Каждый билдер интерпретирует только релевантные ему поля; неприменимые
///          игнорируются. Новые опции добавляются как новые поля — без вмешательства
///          в иерархию IChartBuilder и без касания вызывающих, которые их не используют.
struct BuilderOptions
{
    bool aggregate = true;              ///< Включить агрегацию входных точек; по умолчанию включена.
    const style::IPalette* palette = nullptr;  ///< Палитра для покраски элементов; nullptr — без покраски.
};

/// @brief Интерфейс построителя графика из временного ряда.
class IChartBuilder
{
   public:
    /// @brief Виртуальный деструктор.
    virtual ~IChartBuilder() = default;

    /// @brief Применяет опции к билдеру.
    /// @param[in] options Опции построения; неприменимые поля игнорируются.
    /// @details Базовая реализация — no-op: билдер, не поддерживающий ни одной опции,
    ///          не обязан переопределять метод. Вызывается перед Build().
    virtual void Configure(const BuilderOptions& options) noexcept { (void)options; }

    /// @brief Строит и возвращает QChart на основе данных.
    /// @param[in] data Входные данные временного ряда.
    /// @return Владеющий указатель на построенный QChart.
    [[nodiscard]] virtual std::unique_ptr<QtCharts::QChart> Build(const data::TimelineData& data) = 0;
};

}  // namespace chart

#endif  // GUID_a1b2c3d4_e5f6_7890_abcd_ef1234567890
