/// @file color_style.hpp
/// @brief Объявление цветного стиля оформления графика.
/// @author Artemenko Anton
#ifndef GUID_e5f6a7b8_c9d0_1234_efab_345678901234
#define GUID_e5f6a7b8_c9d0_1234_efab_345678901234

#include <QColor>
#include <QList>
#include <logger/ilogger.hpp>
#include <memory>
#include <style/ichart_style.hpp>
#include <utility>

namespace style
{

/// @brief Цветной стиль — циклическая палитра заданных цветов.
class ColorStyle : public IChartStyle
{
   public:
    /// @brief Конструктор стиля.
    /// @param[in] palette Палитра цветов для циклической раскраски серий.
    /// @param[in] logger Логгер для диагностики применения стиля; допускается nullptr.
    explicit ColorStyle(QList<QColor> palette, const std::shared_ptr<logger::ILogger>& logger = nullptr)
        : palette_(std::move(palette)), logger_(logger)
    {
    }

    /// @brief Возвращает цвет из встроенной палитры по циклическому индексу.
    /// @param[in] index Индекс элемента.
    /// @param[in] total Игнорируется (палитра не зависит от общего числа).
    /// @return Цвет из палитры.
    QColor ColorFor(int index, int total) const override;

    /// @brief Применяет общие настройки графика; покраска идёт через ColorFor() в builder'ах.
    /// @param[in,out] chart График, к которому применяется стиль.
    void Apply(QtCharts::QChart* chart) override;

   private:
    QList<QColor> palette_;                     ///< Палитра цветов для раскраски серий.
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace style

#endif  // GUID_e5f6a7b8_c9d0_1234_efab_345678901234
