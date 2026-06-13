/// @file scrollable_chart_view.cpp
/// @brief Реализация представления графика с прокруткой скроллбарами.
/// @author Artemenko Anton

#include <gui/src/scrollable_chart_view.hpp>

#include <QResizeEvent>
#include <QWheelEvent>
#include <QtCharts/QChart>
#include <QtGlobal>

QT_CHARTS_USE_NAMESPACE

namespace gui
{

namespace
{

/// @brief Шаг изменения масштаба холста за один щелчок колеса (плавный, без резких скачков).
constexpr qreal kScaleStep = 1.15;
/// @brief Минимальный масштаб холста (1.0 — график вписан в окно целиком).
constexpr qreal kMinScale = 1.0;
/// @brief Максимальный масштаб холста (ограничивает чрезмерное увеличение).
constexpr qreal kMaxScale = 6.0;

}  // namespace

ScrollableChartView::ScrollableChartView(QWidget* parent) : QtCharts::QChartView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // Перетаскивание «рукой» прокручивает холст плавно (двигает скроллбары), без зума.
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ScrollableChartView::resetView()
{
    contentScale_ = kMinScale;
    layoutChart();
}

void ScrollableChartView::resizeEvent(QResizeEvent* event)
{
    // Намеренно не вызываем QChartView::resizeEvent: он жёстко подгоняет холст под вьюпорт и не
    // даёт холсту быть крупнее окна. Базовый QGraphicsView обновляет геометрию вьюпорта/скроллбаров.
    QGraphicsView::resizeEvent(event);
    layoutChart();
}

void ScrollableChartView::wheelEvent(QWheelEvent* event)
{
    const qreal previous = contentScale_;
    const qreal factor = event->angleDelta().y() > 0 ? kScaleStep : 1.0 / kScaleStep;
    contentScale_ = qBound(kMinScale, contentScale_ * factor, kMaxScale);
    if (!qFuzzyCompare(previous, contentScale_)) layoutChart();
    event->accept();
}

void ScrollableChartView::layoutChart()
{
    if (chart() == nullptr) return;
    const QSize viewportSize = viewport()->size();
    const QSizeF target(viewportSize.width() * contentScale_, viewportSize.height() * contentScale_);
    chart()->resize(target);
    setSceneRect(QRectF(QPointF(0.0, 0.0), target));
}

}  // namespace gui
