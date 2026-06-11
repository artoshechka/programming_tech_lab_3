/// @file file_item_delegate.cpp
/// @brief Реализация двухстрочного делегата дерева файлов.
/// @author Artemenko Anton

#include <gui/src/file_item_delegate.hpp>

#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFont>
#include <QIcon>
#include <QPainter>

namespace gui
{

namespace
{

/// @brief Человекочитаемый размер файла.
/// @param[in] bytes Размер в байтах.
/// @return Строка вида "162 КБ".
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
    base.setHeight(isDir ? 34 : 48);
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

    const QColor nameColor = selected ? (dark_ ? QColor(0xf5, 0xb6, 0xa8) : QColor(0x8c, 0x1a, 0x0f))
                                       : (dark_ ? QColor(0xec, 0xec, 0xee) : QColor(0x2b, 0x2b, 0x30));
    const QColor metaColor = dark_ ? QColor(0x82, 0x82, 0x8a) : QColor(0xa6, 0xa6, 0xae);

    const QRect r = option.rect;
    const int iconSize = 18;
    const int leftPad = 8;
    QRect iconRect(r.left() + leftPad, r.top() + (r.height() - iconSize) / 2, iconSize, iconSize);
    const QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    if (!icon.isNull()) icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    const int textLeft = iconRect.right() + 10;
    const QRect textRect(textLeft, r.top(), r.right() - textLeft - 10, r.height());

    const QString name = index.data(Qt::DisplayRole).toString();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    const bool isDir = (model != nullptr) && model->isDir(index);
    if (isDir || model == nullptr)
    {
        QFont nameFont = option.font;
        nameFont.setWeight(selected ? QFont::DemiBold : QFont::Normal);
        painter->setFont(nameFont);
        painter->setPen(nameColor);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, name);
        painter->restore();
        return;
    }

    const QFileInfo info = model->fileInfo(index);
    const QString meta = info.lastModified().toString("dd.MM.yyyy HH:mm") + "   ·   " + HumanSize(info.size());

    QFont nameFont = option.font;
    nameFont.setWeight(selected ? QFont::DemiBold : QFont::Normal);
    painter->setFont(nameFont);
    painter->setPen(nameColor);
    const QRect nameRect(textLeft, r.top() + 6, textRect.width(), r.height() / 2);
    painter->drawText(nameRect, Qt::AlignBottom | Qt::AlignLeft, name);

    QFont metaFont = option.font;
    metaFont.setPointSizeF(qMax(7.0, option.font.pointSizeF() - 1.5));
    painter->setFont(metaFont);
    painter->setPen(metaColor);
    const QRect metaRect(textLeft, r.center().y() + 1, textRect.width(), r.height() / 2 - 4);
    painter->drawText(metaRect, Qt::AlignTop | Qt::AlignLeft, meta);

    painter->restore();
}

}  // namespace gui
