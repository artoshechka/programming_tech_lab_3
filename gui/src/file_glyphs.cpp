/// @file file_glyphs.cpp
/// @brief Реализация векторных глифов иконок файлов/папок.
/// @author Artemenko Anton

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <gui/src/file_glyphs.hpp>

namespace gui
{

namespace
{

/// @brief Сторона канвы, в координатах которой заданы все контуры глифов.
constexpr qreal kGlyphCanvas = 16.0;
/// @brief Толщина контура глифа (в координатах канвы).
constexpr qreal kGlyphPenWidth = 1.2;

/// @brief Контур страницы документа (со скруглениями и загнутым уголком) в координатах 16×16.
/// @details Общая основа для иконок JSON и прочих файлов; содержимое рисуется поверх отдельно.
QPainterPath PageBodyPath()
{
    QPainterPath p;
    p.moveTo(4.4, 1.8);
    p.lineTo(9.2, 1.8);
    p.lineTo(12.4, 5.0);
    p.lineTo(12.4, 13.4);
    p.cubicTo(12.4, 14.1, 11.9, 14.4, 11.4, 14.4);
    p.lineTo(4.4, 14.4);
    p.cubicTo(3.9, 14.4, 3.4, 14.1, 3.4, 13.4);
    p.lineTo(3.4, 2.8);
    p.cubicTo(3.4, 2.1, 3.9, 1.8, 4.4, 1.8);
    p.closeSubpath();
    // Загнутый уголок.
    p.moveTo(9.2, 1.9);
    p.lineTo(9.2, 5.0);
    p.lineTo(12.3, 5.0);
    return p;
}

/// @brief Глиф обычного файла: страница с тремя строками текста.
QPainterPath GenericGlyphPath()
{
    QPainterPath p = PageBodyPath();
    p.moveTo(5.4, 8.4);
    p.lineTo(10.2, 8.4);
    p.moveTo(5.4, 10.4);
    p.lineTo(10.2, 10.4);
    p.moveTo(5.4, 12.2);
    p.lineTo(8.6, 12.2);
    return p;
}

/// @brief Глиф JSON-файла: страница с фигурными скобками { }.
QPainterPath JsonGlyphPath()
{
    QPainterPath p = PageBodyPath();
    // Левая скобка.
    p.moveTo(7.3, 7.4);
    p.cubicTo(6.3, 7.4, 6.9, 9.6, 6.0, 10.2);
    p.cubicTo(6.9, 10.8, 6.3, 13.0, 7.3, 13.0);
    // Правая скобка.
    p.moveTo(8.9, 7.4);
    p.cubicTo(9.9, 7.4, 9.3, 9.6, 10.2, 10.2);
    p.cubicTo(9.3, 10.8, 9.9, 13.0, 8.9, 13.0);
    return p;
}

/// @brief Глиф базы SQLite: цилиндр БД с горизонтальными полосами.
/// @details Передние полудуги строятся через cubicTo (выгнуты вниз) — у arcTo направление
///          дуги в координатах с осью Y вниз неинтуитивно и давало паразитные пересечения.
QPainterPath SqliteGlyphPath()
{
    QPainterPath p;
    const qreal cx = 8.0, rx = 4.2, ry = 1.6, top = 3.6, bot = 12.4;
    const qreal k = ry * 1.35;  // вынос контрольных точек для полуэллипса

    // Передняя (нижняя) полудуга на уровне y: слева направо, выгнута вниз.
    auto frontArc = [&](qreal y) {
        p.moveTo(cx - rx, y);
        p.cubicTo(cx - rx, y + k, cx + rx, y + k, cx + rx, y);
    };

    // Верхний эллипс (полный).
    p.addEllipse(QPointF(cx, top), rx, ry);
    // Боковые стенки.
    p.moveTo(cx - rx, top);
    p.lineTo(cx - rx, bot);
    p.moveTo(cx + rx, top);
    p.lineTo(cx + rx, bot);
    // Нижнее дно и две промежуточные полосы.
    frontArc(bot);
    frontArc(top + 3.0);
    frontArc(top + 6.0);
    return p;
}

/// @brief Контурный глиф папки в системе координат 16×16.
QPainterPath FolderGlyphPath()
{
    QPainterPath p;
    p.moveTo(1.5, 4.2);
    p.cubicTo(1.5, 3.6, 1.9, 3.2, 2.5, 3.2);
    p.lineTo(6.0, 3.2);
    p.lineTo(7.3, 4.5);
    p.lineTo(13.5, 4.5);
    p.cubicTo(14.0, 4.5, 14.5, 4.9, 14.5, 5.5);
    p.lineTo(14.5, 12.3);
    p.cubicTo(14.5, 12.8, 14.0, 13.3, 13.5, 13.3);
    p.lineTo(2.5, 13.3);
    p.cubicTo(1.9, 13.3, 1.5, 12.8, 1.5, 12.3);
    p.closeSubpath();
    return p;
}

/// @brief Возвращает контур иконки для заданного типа файла.
QPainterPath GlyphPathFor(FileKind kind)
{
    switch (kind)
    {
        case FileKind::Folder:
            return FolderGlyphPath();
        case FileKind::Json:
            return JsonGlyphPath();
        case FileKind::Sqlite:
            return SqliteGlyphPath();
        case FileKind::Generic:
            break;
    }
    return GenericGlyphPath();
}

}  // namespace

FileKind ClassifyFileKind(bool isDir, const QString& suffixLower)
{
    if (isDir) return FileKind::Folder;
    if (suffixLower == "json") return FileKind::Json;
    if (suffixLower == "sqlite" || suffixLower == "sqlite3" || suffixLower == "db") return FileKind::Sqlite;
    return FileKind::Generic;
}

void PaintFileGlyph(QPainter* painter, const QRectF& rect, FileKind kind, const QColor& color)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(rect.topLeft());
    const qreal scale = rect.width() / kGlyphCanvas;
    painter->scale(scale, scale);
    QPen pen(color, kGlyphPenWidth);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(GlyphPathFor(kind));
    painter->restore();
}

}  // namespace gui
