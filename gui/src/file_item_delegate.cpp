/// @file file_item_delegate.cpp
/// @brief Реализация делегата строки дерева файлов (высота строки + фон/выделение).
/// @author Artemenko Anton

#include <gui/src/file_item_delegate.hpp>

#include <QApplication>
#include <QFileSystemModel>
#include <QIcon>
#include <QPainter>

#include <gui/src/file_row_widget.hpp>

namespace gui
{

FileItemDelegate::FileItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QSize FileItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const auto* model = qobject_cast<const QFileSystemModel*>(index.model());
    const bool isDir = (model != nullptr) && model->isDir(index);
    QSize base = QStyledItemDelegate::sizeHint(option, index);
    base.setHeight(isDir ? kDirRowHeight : kFileRowHeight);
    return base;
}

void FileItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Содержимое строки рисует FileRowWidget (setIndexWidget); делегат рисует только фон и
    // выделение штатным стилем, очистив текст и иконку, чтобы они не дублировались под виджетом.
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.text.clear();
    opt.icon = QIcon();
    opt.features &= ~QStyleOptionViewItem::HasDecoration;
    const QWidget* widget = option.widget;
    QStyle* style = (widget != nullptr) ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

}  // namespace gui
