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
#include <QtSvg/QSvgRenderer>

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

/// @brief SVG-глиф файла (контурный).
constexpr const char* kFileSvg =
    "<svg viewBox='0 0 16 16' fill='none'><path d='M4 1.6h5l3 3v9.8c0 .3-.3.6-.6.6H4c-.4 0-.6-.3-.6-.6V2.2c0-.3.2-.6.6-.6Z' "
    "stroke='%1' stroke-width='1.2'/><path d='M9 1.8v3h3' stroke='%1' stroke-width='1.2'/></svg>";

/// @brief SVG-глиф папки (контурный).
constexpr const char* kFolderSvg =
    "<svg viewBox='0 0 16 16' fill='none'><path d='M1.5 4.2c0-.6.4-1 1-1H6l1.3 1.3h6.2c.5 0 1 .4 1 1v6.8c0 .5-.5 "
    "1-1 1H2.5c-.6 0-1-.5-1-1V4.2Z' stroke='%1' stroke-width='1.2'/></svg>";

/// @brief Рендерит SVG-строку (с подстановкой цвета) в пиксмап с учётом DPR (резко на Retina).
QPixmap RenderSvg(const char* svg, const QColor& color, int size, qreal dpr)
{
    const QString src = QString(svg).arg(color.name());
    QSvgRenderer renderer(src.toUtf8());
    QPixmap pm(qRound(size * dpr), qRound(size * dpr));
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    renderer.render(&p, QRectF(0, 0, size * dpr, size * dpr));
    p.end();
    pm.setDevicePixelRatio(dpr);
    return pm;
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
    base.setHeight(isDir ? 36 : 52);
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
    const qreal dpr = (painter->device() != nullptr) ? painter->device()->devicePixelRatioF() : 1.0;
    const QRect iconRect(r.left() + leftPad, r.top() + (r.height() - iconSize) / 2, iconSize, iconSize);
    painter->drawPixmap(iconRect, RenderSvg(isDir ? kFolderSvg : kFileSvg, iconColor, iconSize, dpr));

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
    metaFont.setPointSizeF(qMax(7.0, option.font.pointSizeF() - 2.0));
    painter->setFont(metaFont);
    painter->setPen(metaColor);
    painter->drawText(QRect(textLeft, mid + 2, textRight - textLeft, r.bottom() - mid - 4),
                      Qt::AlignTop | Qt::AlignLeft, meta);

    painter->restore();
}

}  // namespace gui
