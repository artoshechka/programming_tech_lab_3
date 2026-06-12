/// @file file_item_delegate.cpp
/// @brief Реализация двухстрочного делегата дерева файлов.
/// @author Artemenko Anton

#include <gui/src/file_item_delegate.hpp>

#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFont>
#include <QPainter>
#include <QPainterPath>

namespace gui
{

namespace
{

/// @brief Человекочитаемый размер файла.
QString HumanSize(qint64 bytes)
{
    static const char* kUnits[] = {"Б", "КБ", "МБ", "ГБ"};
    double size = static_cast<double>(bytes);
    int unit = 0;
    while (size >= 1024.0 && unit < 3)
    {
        size /= 1024.0;
        ++unit;
    }
    return QString::number(size, 'f', unit == 0 ? 0 : 1) + " " + kUnits[unit];
}

/// @brief Контурный глиф файла в системе координат 16×16 (совпадает с прежним SVG).
QPainterPath FileGlyphPath()
{
    QPainterPath p;
    p.moveTo(4.0, 1.6);
    p.lineTo(9.0, 1.6);
    p.lineTo(12.0, 4.6);
    p.lineTo(12.0, 14.4);
    p.cubicTo(12.0, 14.7, 11.7, 15.0, 11.4, 15.0);
    p.lineTo(4.0, 15.0);
    p.cubicTo(3.6, 15.0, 3.4, 14.7, 3.4, 14.4);
    p.lineTo(3.4, 2.2);
    p.cubicTo(3.4, 1.9, 3.6, 1.6, 4.0, 1.6);
    p.closeSubpath();
    p.moveTo(9.0, 1.8);
    p.lineTo(9.0, 4.8);
    p.lineTo(12.0, 4.8);
    return p;
}

/// @brief Контурный глиф папки в системе координат 16×16 (совпадает с прежним SVG).
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

/// @brief Рисует контурную иконку файла/папки прямо на painter (вектор, DPR-резкость от Qt).
void DrawGlyph(QPainter* painter, const QRect& rect, bool isDir, const QColor& color)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(rect.topLeft());
    const qreal scale = rect.width() / 16.0;
    painter->scale(scale, scale);
    painter->setPen(QPen(color, 1.2));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(isDir ? FolderGlyphPath() : FileGlyphPath());
    painter->restore();
}

}  // namespace

FileItemDelegate::FileItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void FileItemDelegate::setDark(bool dark)
{
    dark_ = dark;
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const auto* model = qobject_cast<const QFileSystemModel*>(index.model());
    const bool isDir = (model != nullptr) && model->isDir(index);
    QSize base = QStyledItemDelegate::sizeHint(option, index);
    base.setHeight(isDir ? 36 : 56);
    return base;
}

void FileItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Фон и выделение рисует стиль (через QSS), но без штатных иконки/текста — их рисуем сами.
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.text.clear();
    opt.icon = QIcon();
    opt.features &= ~QStyleOptionViewItem::HasDecoration;
    const QWidget* widget = option.widget;
    QStyle* style = (widget != nullptr) ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    const auto* model = qobject_cast<const QFileSystemModel*>(index.model());
    const bool selected = (option.state & QStyle::State_Selected) != 0;
    const bool isDir = (model != nullptr) && model->isDir(index);

    const QColor selName = dark_ ? accent_.lighter(140) : accent_.darker(135);
    const QColor nameColor = selected ? selName : (dark_ ? QColor(0xec, 0xec, 0xee) : QColor(0x2b, 0x2b, 0x30));
    const QColor metaColor = dark_ ? QColor(0x82, 0x82, 0x8a) : QColor(0xa6, 0xa6, 0xae);
    const QColor iconColor = selected ? accent_ : metaColor;

    const QRect r = option.rect;
    const int iconSize = 17;
    const int leftPad = 8;
    const QRect iconRect(r.left() + leftPad, r.top() + (r.height() - iconSize) / 2, iconSize, iconSize);
    DrawGlyph(painter, iconRect, isDir, iconColor);

    const int textLeft = iconRect.right() + 10;
    const int textRight = r.right() - 12;
    const QString name = index.data(Qt::DisplayRole).toString();

    painter->save();
    if (isDir || model == nullptr)
    {
        QFont nameFont = option.font;
        nameFont.setWeight(selected ? QFont::DemiBold : QFont::Normal);
        painter->setFont(nameFont);
        painter->setPen(nameColor);
        painter->drawText(QRect(textLeft, r.top(), textRight - textLeft, r.height()), Qt::AlignVCenter | Qt::AlignLeft,
                          name);
        painter->restore();
        return;
    }

    const QFileInfo info = model->fileInfo(index);
    const QString meta = info.lastModified().toString("dd.MM.yyyy HH:mm") + "   ·   " + HumanSize(info.size());
    const int mid = r.top() + r.height() / 2;

    QFont nameFont = option.font;
    nameFont.setWeight(selected ? QFont::DemiBold : QFont::Normal);
    painter->setFont(nameFont);
    painter->setPen(nameColor);
    painter->drawText(QRect(textLeft, r.top() + 4, textRight - textLeft, mid - r.top() - 4),
                      Qt::AlignBottom | Qt::AlignLeft, name);

    QFont metaFont = option.font;
    metaFont.setPointSizeF(qMax(8.0, option.font.pointSizeF() - 0.5));
    painter->setFont(metaFont);
    painter->setPen(metaColor);
    painter->drawText(QRect(textLeft, mid + 2, textRight - textLeft, r.bottom() - mid - 4),
                      Qt::AlignTop | Qt::AlignLeft, meta);

    painter->restore();
}

}  // namespace gui
