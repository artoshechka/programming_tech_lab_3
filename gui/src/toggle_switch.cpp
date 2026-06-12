/// @file toggle_switch.cpp
/// @brief Реализация анимированного переключателя-ползунка.
/// @author Artemenko Anton

#include <gui/src/toggle_switch.hpp>

#include <QPainter>
#include <QPropertyAnimation>

namespace gui
{

namespace
{
constexpr int kWidth = 42;    ///< Ширина виджета.
constexpr int kHeight = 22;   ///< Высота виджета.
constexpr int kMargin = 3;    ///< Отступ ползунка от края дорожки.
}  // namespace

ToggleSwitch::ToggleSwitch(QWidget* parent) : QAbstractButton(parent)
{
    setCheckable(true);
    setCursor(Qt::PointingHandCursor);
    connect(this, &QAbstractButton::toggled, this, [this](bool on) {
        auto* anim = new QPropertyAnimation(this, "pos", this);
        anim->setDuration(140);
        anim->setStartValue(pos_);
        anim->setEndValue(on ? 1.0 : 0.0);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

QSize ToggleSwitch::sizeHint() const
{
    return {kWidth, kHeight};
}

void ToggleSwitch::setPos(qreal p)
{
    pos_ = p;
    update();
}

void ToggleSwitch::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal h = kHeight;
    const qreal radius = h / 2.0;
    const int thumbD = kHeight - 2 * kMargin;

    // Дорожка: акцент во включённом состоянии, нейтральный серый — в выключенном.
    QColor track = isEnabled() ? QColor(0xc9, 0xc8, 0xc6) : QColor(0xe0, 0xdf, 0xdd);
    if (pos_ > 0.0)
    {
        QColor on = accent_;
        track = QColor::fromRgbF(track.redF() + (on.redF() - track.redF()) * pos_,
                                 track.greenF() + (on.greenF() - track.greenF()) * pos_,
                                 track.blueF() + (on.blueF() - track.blueF()) * pos_);
    }
    p.setPen(Qt::NoPen);
    p.setBrush(track);
    p.drawRoundedRect(0, 0, kWidth, kHeight, radius, radius);

    // Ползунок.
    const qreal travel = kWidth - thumbD - 2 * kMargin;
    const qreal x = kMargin + pos_ * travel;
    p.setBrush(QColor(0xff, 0xff, 0xff));
    p.drawEllipse(QRectF(x, kMargin, thumbD, thumbD));
}

}  // namespace gui
