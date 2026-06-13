/// @file file_item_delegate.hpp
/// @brief Делегат строки дерева файлов: задаёт высоту строки и рисует только фон/выделение.
/// @author Artemenko Anton
#ifndef GUID_07182939_4a5b_4d16_0718_2939_4a5b6c7d
#define GUID_07182939_4a5b_4d16_0718_2939_4a5b6c7d

#include <QStyledItemDelegate>

namespace gui
{

/// @brief Делегат строки дерева файлов.
/// @details Содержимое строки (иконку, имя, мету) рисует живой виджет FileRowWidget,
///          установленный через QTreeView::setIndexWidget. Делегат отвечает лишь за высоту
///          строки (двухстрочная для файлов, однострочная для папок) и за отрисовку
///          фона/выделения штатным стилем без дублирующего текста.
class FileItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

   public:
    /// @brief Конструктор делегата.
    /// @param[in] parent Родительский QObject (по умолчанию nullptr).
    explicit FileItemDelegate(QObject* parent = nullptr);

    /// @brief Рекомендуемый размер строки (двухстрочный для файлов, однострочный для папок).
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /// @brief Рисует только фон/выделение строки штатным стилем (без текста и иконки).
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}  // namespace gui

#endif  // GUID_07182939_4a5b_4d16_0718_2939_4a5b6c7d
