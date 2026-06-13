/// @file scrollable_chart_view.hpp
/// @brief QChartView с прокруткой графика скроллбарами вместо резкого зума диапазона.
/// @author Artemenko Anton
/// @details Решает проблему «схлопнутых» подписей осей (… вместо значений). Холст графика может
///          быть крупнее вьюпорта: тогда подписи осей разъезжаются и читаются, а перемещение по
///          ним выполняется родными скроллбарами QGraphicsView (плавно, постранично) и
///          перетаскиванием «рукой». Колесо плавно меняет масштаб холста (во сколько раз он
///          крупнее окна), не трогая диапазоны осей. Работает для любого типа графика.
#ifndef GUID_8e3b07a4_1f6c_4d52_9a83_2b5e64c1f0d9
#define GUID_8e3b07a4_1f6c_4d52_9a83_2b5e64c1f0d9

#include <QtCharts/QChartView>

namespace gui
{

/// @brief Представление графика с прокруткой скроллбарами и масштабированием холста колесом.
class ScrollableChartView : public QtCharts::QChartView
{
    Q_OBJECT

   public:
    /// @brief Конструктор представления.
    /// @param[in] parent Родительский виджет (по умолчанию nullptr).
    explicit ScrollableChartView(QWidget* parent = nullptr);

    /// @brief Сбрасывает масштаб холста к вписанному в окно (вызывается при смене графика).
    void resetView();

   protected:
    /// @brief Переразмещает холст под новый размер вьюпорта с учётом текущего масштаба.
    void resizeEvent(QResizeEvent* event) override;
    /// @brief Колесо мыши — плавное масштабирование холста (крупнее/мельче), без зума диапазона.
    void wheelEvent(QWheelEvent* event) override;

   private:
    /// @brief Задаёт размер холста графика = размер вьюпорта × текущий масштаб и область прокрутки.
    void layoutChart();

    qreal contentScale_ = 1.0;  ///< Во сколько раз холст графика крупнее вьюпорта (1.0 — вписан).
};

}  // namespace gui

#endif  // GUID_8e3b07a4_1f6c_4d52_9a83_2b5e64c1f0d9
