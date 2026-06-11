/// @file toggle_switch.hpp
/// @brief Анимированный переключатель-ползунок (замена квадратного QCheckBox).
/// @author Artemenko Anton
#ifndef GUID_f6071829_3a4b_4c15_f607_18293a4b5c6d
#define GUID_f6071829_3a4b_4c15_f607_18293a4b5c6d

#include <QAbstractButton>
#include <QColor>

namespace gui
{

/// @brief Переключатель в виде скользящего ползунка (track + thumb) с плавной анимацией.
/// @details Checkable-кнопка: состояние читается через isChecked(), меняется кликом.
///          Положение ползунка анимируется свойством pos в диапазоне [0; 1].
class ToggleSwitch : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(qreal pos READ pos WRITE setPos)

   public:
    /// @brief Конструктор переключателя.
    /// @param[in] parent Родительский виджет (по умолчанию nullptr).
    explicit ToggleSwitch(QWidget* parent = nullptr);

    /// @brief Рекомендуемый размер виджета.
    QSize sizeHint() const override;

    /// @brief Текущее положение ползунка [0; 1] (для анимации).
    qreal pos() const
    {
        return pos_;
    }
    /// @brief Задаёт положение ползунка и перерисовывает.
    /// @param[in] p Положение в диапазоне [0; 1].
    void setPos(qreal p);

   protected:
    /// @brief Рисует дорожку и ползунок.
    void paintEvent(QPaintEvent* event) override;

   private:
    qreal pos_ = 0.0;             ///< Положение ползунка: 0 — выкл, 1 — вкл.
    QColor accent_{0xc0, 0x28, 0x1a};  ///< Цвет включённого состояния (оранжевый акцент).
};

}  // namespace gui

#endif  // GUID_f6071829_3a4b_4c15_f607_18293a4b5c6d
