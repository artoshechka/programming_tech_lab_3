/// @file file_row_widget.hpp
/// @brief Живой виджет строки дерева файлов (иконка + имя + мета), компонуемый QLayout.
/// @author Artemenko Anton
/// @details Заменяет ручную отрисовку координат в делегате: строка собирается из настоящих
///          QLabel в QLayout, поэтому размеры колонок и переносы считает Qt, а не хардкод.
///          Виджет прозрачен для мыши — выделение и клики обрабатывает само дерево.
#ifndef GUID_5d8a1f63_2c47_4e90_b6a1_7f3e02c9d845
#define GUID_5d8a1f63_2c47_4e90_b6a1_7f3e02c9d845

#include <QColor>
#include <QPersistentModelIndex>
#include <QWidget>
#include <gui/src/file_glyphs.hpp>

class QFileInfo;
class QLabel;

namespace gui
{

/// @brief Высота строки файла (две строки: имя + мета).
inline constexpr int kFileRowHeight = 56;
/// @brief Высота строки папки (одна строка: имя).
inline constexpr int kDirRowHeight = 36;

class GlyphIcon;

/// @brief Строка дерева файлов: иконка слева, имя и (для файлов) строка меты «размер · дата».
/// @details Цвета и насыщенность шрифта подстраиваются под тему и состояние выделения.
class FileRowWidget : public QWidget
{
    Q_OBJECT

   public:
    /// @brief Конструктор строки.
    /// @param[in] kind Тип файла (для иконки).
    /// @param[in] isDir Папка ли это (определяет одно- или двухстрочную компоновку).
    /// @param[in] name Отображаемое имя элемента.
    /// @param[in] info Сведения о файле (размер, дата) — используются только для файлов.
    /// @param[in] index Индекс элемента модели (для сопоставления с выделением).
    /// @param[in] parent Родительский виджет.
    FileRowWidget(FileKind kind, bool isDir, const QString& name, const QFileInfo& info, const QModelIndex& index,
                  QWidget* parent = nullptr);

    /// @brief Помечает строку выделенной/невыделенной (меняет цвет и насыщенность шрифта).
    /// @param[in] selected true — строка выделена.
    void setSelected(bool selected);

    /// @brief Применяет тему: пересчитывает цвета текста и иконки.
    /// @param[in] dark true — тёмная тема.
    /// @param[in] accent Акцентный цвет (для выделенной строки и иконки).
    void applyTheme(bool dark, const QColor& accent);

    /// @brief Индекс модели, которому соответствует строка.
    QModelIndex index() const
    {
        return index_;
    }

   private:
    /// @brief Пересчитывает цвета/шрифты лейблов и цвет иконки по текущим теме и выделению.
    void updateColors();

    FileKind kind_;                ///< Тип файла (для иконки).
    bool isDir_;                   ///< Папка ли это.
    bool selected_ = false;        ///< Выделена ли строка.
    bool dark_ = false;            ///< Активна ли тёмная тема.
    QColor accent_;                ///< Акцентный цвет.
    QPersistentModelIndex index_;  ///< Индекс элемента модели.
    GlyphIcon* icon_ = nullptr;    ///< Виджет-иконка.
    QLabel* nameLabel_ = nullptr;  ///< Имя элемента.
    QLabel* sizeLabel_ = nullptr;  ///< Размер файла (только для файлов).
    QLabel* dateLabel_ = nullptr;  ///< Дата изменения (только для файлов).
};

}  // namespace gui

#endif  // GUID_5d8a1f63_2c47_4e90_b6a1_7f3e02c9d845
