/// @file file_row_widget.cpp
/// @brief Реализация живого виджета строки дерева файлов.
/// @author Artemenko Anton

#include <QDateTime>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QRectF>
#include <QVBoxLayout>
#include <memory>
#include <gui/src/file_row_widget.hpp>

namespace gui
{

namespace
{

/// @brief Левый отступ строки до иконки.
constexpr int kRowLeftPad = 8;
/// @brief Правый отступ строки.
constexpr int kRowRightPad = 12;
/// @brief Зазор между иконкой и текстовой колонкой.
constexpr int kIconTextGap = 10;
/// @brief Сторона бокса иконки (область виджета-иконки).
constexpr int kIconBox = 24;
/// @brief Сторона самого глифа внутри бокса (центрируется).
constexpr int kIconGlyph = 18;
/// @brief Вертикальный зазор между именем и строкой меты.
constexpr int kNameMetaGap = 2;
/// @brief Горизонтальный зазор между размером и датой в строке меты.
constexpr int kMetaGap = 14;
/// @brief Уменьшение кегля строки меты относительно базового шрифта.
constexpr qreal kMetaPointDelta = 0.5;
/// @brief Минимальный кегль строки меты.
constexpr qreal kMetaMinPoint = 8.0;
/// @brief Формат даты изменения в строке меты.
constexpr const char* kMetaDateFormat = "dd.MM.yyyy HH:mm";

// Цвета текста по теме (совпадают с прежней палитрой делегата).
const QColor kNameLight(0x2b, 0x2b, 0x30);  ///< Имя, светлая тема.
const QColor kNameDark(0xec, 0xec, 0xee);   ///< Имя, тёмная тема.
const QColor kMetaLight(0xa6, 0xa6, 0xae);  ///< Мета, светлая тема.
const QColor kMetaDark(0x82, 0x82, 0x8a);   ///< Мета, тёмная тема.

/// @brief Человекочитаемый размер файла.
/// @param[in] bytes Размер в байтах.
/// @return Строка вида «976.0 КБ».
QString HumanSize(qint64 bytes)
{
    static const char* kUnits[] = {"Б", "КБ", "МБ", "ГБ"};
    constexpr double kStep = 1024.0;
    constexpr int kLastUnit = 3;
    double size = static_cast<double>(bytes);
    int unit = 0;
    while (size >= kStep && unit < kLastUnit)
    {
        size /= kStep;
        ++unit;
    }
    return QString::number(size, 'f', unit == 0 ? 0 : 1) + " " + kUnits[unit];
}

/// @brief Прозрачная для мыши CSS-строка цвета лейбла.
QString LabelCss(const QColor& color)
{
    return QStringLiteral("color:%1; background:transparent;").arg(color.name());
}

}  // namespace

/// @brief Виджет-иконка: рисует векторный глиф файла/папки по центру своего бокса.
/// @details Без Q_OBJECT — переопределяет только paintEvent, сигналов/слотов не имеет.
class GlyphIcon : public QWidget
{
   public:
    /// @brief Конструктор иконки.
    /// @param[in] kind Тип файла.
    /// @param[in] parent Родительский виджет.
    explicit GlyphIcon(FileKind kind, QWidget* parent = nullptr) : QWidget(parent), kind_(kind)
    {
        setObjectName("fileRowIcon");
        setFixedSize(kIconBox, kIconBox);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    /// @brief Задаёт цвет контура и перерисовывает иконку.
    /// @param[in] color Цвет контура.
    void setColor(const QColor& color)
    {
        color_ = color;
        update();
    }

   protected:
    /// @brief Рисует глиф по центру бокса.
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        const qreal inset = (kIconBox - kIconGlyph) / 2.0;
        PaintFileGlyph(&painter, QRectF(inset, inset, kIconGlyph, kIconGlyph), kind_, color_);
    }

   private:
    FileKind kind_;  ///< Тип файла.
    QColor color_;   ///< Цвет контура.
};

FileRowWidget::FileRowWidget(FileKind kind, bool isDir, const QString& name, const QFileInfo& info,
                             const QModelIndex& index, QWidget* parent)
    : QWidget(parent), kind_(kind), isDir_(isDir), index_(index)
{
    setObjectName("fileRow");
    // Выделение и клики обрабатывает дерево — строка не перехватывает мышь.
    setAttribute(Qt::WA_TransparentForMouseEvents);

    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(kRowLeftPad, 0, kRowRightPad, 0);
    row->setSpacing(kIconTextGap);

    icon_ = new GlyphIcon(kind, this);
    row->addWidget(icon_, 0, Qt::AlignVCenter);

    nameLabel_ = new QLabel(name, this);
    nameLabel_->setObjectName("fileRowName");
    nameLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);

    if (isDir_)
    {
        // Папка — одна строка с именем по центру высоты.
        row->addWidget(nameLabel_, 1, Qt::AlignVCenter);
    } else
    {
        // Файл — имя сверху, под ним мета «размер · дата».
        auto col = std::make_unique<QVBoxLayout>();
        col->setContentsMargins(0, 0, 0, 0);
        col->setSpacing(kNameMetaGap);
        col->addStretch();
        col->addWidget(nameLabel_);

        auto meta = std::make_unique<QHBoxLayout>();
        meta->setContentsMargins(0, 0, 0, 0);
        meta->setSpacing(kMetaGap);
        sizeLabel_ = new QLabel(HumanSize(info.size()), this);
        dateLabel_ = new QLabel(info.lastModified().toString(kMetaDateFormat), this);
        for (QLabel* label : {sizeLabel_, dateLabel_})
        {
            label->setAttribute(Qt::WA_TransparentForMouseEvents);
            meta->addWidget(label);
        }
        meta->addStretch();
        col->addLayout(meta.release());
        col->addStretch();
        row->addLayout(col.release(), 1);
    }

    updateColors();
}

void FileRowWidget::setSelected(bool selected)
{
    if (selected == selected_) return;
    selected_ = selected;
    updateColors();
}

void FileRowWidget::applyTheme(bool dark, const QColor& accent)
{
    dark_ = dark;
    accent_ = accent;
    updateColors();
}

void FileRowWidget::updateColors()
{
    const QColor selName = dark_ ? accent_.lighter(140) : accent_.darker(135);
    const QColor nameColor = selected_ ? selName : (dark_ ? kNameDark : kNameLight);
    const QColor metaColor = dark_ ? kMetaDark : kMetaLight;
    const QColor iconColor = selected_ ? accent_ : metaColor;

    QFont nameFont = font();
    nameFont.setWeight(selected_ ? QFont::DemiBold : QFont::Normal);
    nameLabel_->setFont(nameFont);
    nameLabel_->setStyleSheet(LabelCss(nameColor));

    if (sizeLabel_ != nullptr)
    {
        QFont metaFont = font();
        metaFont.setPointSizeF(qMax(kMetaMinPoint, font().pointSizeF() - kMetaPointDelta));
        const QString css = LabelCss(metaColor);
        sizeLabel_->setFont(metaFont);
        sizeLabel_->setStyleSheet(css);
        dateLabel_->setFont(metaFont);
        dateLabel_->setStyleSheet(css);
    }

    icon_->setColor(iconColor);
}

}  // namespace gui
