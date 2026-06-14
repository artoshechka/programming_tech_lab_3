/// @file pdf_exporter.cpp
/// @brief Определение экспортёра графика в PDF.
/// @author Artemenko Anton

#include <gui/src/pdf_exporter.hpp>

#include <QGraphicsScene>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <gui/ui_strings.hpp>

namespace gui
{

/// @brief Рендерит всю сцену графика на страницу A4 PDF.
/// @details Рендерится вся сцена, а не вьюпорт: иначе при прокрутке/масштабе в файл попал бы
///          только видимый обрезок. render() сцены не зависит от положения скроллбаров.
bool PdfExporter::Export(QGraphicsScene& scene, const QString& path)
{
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);
    if (!painter.isActive()) return false;
    scene.render(&painter);
    painter.end();
    return true;
}

QString PdfExporter::FileFilter() const
{
    return ui::kPdfFilter;
}

}  // namespace gui
