/// @file file_item_delegate.hpp
/// @brief Делегат отрисовки файла в дереве: имя + мета (дата изменения, размер).
/// @author Artemenko Anton
#ifndef GUID_07182939_4a5b_4d16_0718_2939_4a5b6c7d
#define GUID_07182939_4a5b_4d16_0718_2939_4a5b6c7d

#include <QStyledItemDelegate>

namespace gui
{

/// @brief Двухстрочный делегат элемента дерева файлов.
/// @details Для файлов рисует имя и под ним приглушённую мету «дата · размер»;
///          для папок — одну строку с именем. Цвета подстраиваются под тему.
class FileItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

   public:
    /// @brief Конструктор делегата.
    /// @param[in] parent Родительский QObject (по умолчанию nullptr).
    explicit FileItemDelegate(QObject* parent = nullptr);

    /// @brief Переключает цветовую схему делегата (светлая/тёмная).
    /// @param[in] dark true — тёмная тема.
    void setDark(bool dark);

    /// @brief Рекомендуемый размер элемента (двухстрочный для файлов).
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /// @brief Рисует элемент: фон/выделение через стиль, поверх — иконку и текст.
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

   private:
    bool dark_ = false;  ///< Активна ли тёмная тема.
};

}  // namespace gui

#endif  // GUID_07182939_4a5b_4d16_0718_2939_4a5b6c7d
