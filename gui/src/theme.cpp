/// @file theme.cpp
/// @brief Определение светлой и тёмной тем (QSS) с настраиваемым акцентом.
/// @author Artemenko Anton
/// @details Акцент задаётся снаружи (цвет выбранной палитры); производные оттенки
///          (hover/pressed/мягкая подложка/текст выделения) вычисляются автоматически.

#include <gui/src/theme.hpp>

namespace gui::theme
{

namespace
{

/// @brief Линейное смешение двух цветов.
/// @param[in] a Первый цвет.
/// @param[in] b Второй цвет.
/// @param[in] t Доля второго цвета [0; 1].
QColor Mix(const QColor& a, const QColor& b, double t)
{
    return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t, a.greenF() + (b.greenF() - a.greenF()) * t,
                            a.blueF() + (b.blueF() - a.blueF()) * t);
}

/// @brief Относительная яркость цвета [0; 1].
double Luma(const QColor& c)
{
    return 0.299 * c.redF() + 0.587 * c.greenF() + 0.114 * c.blueF();
}

/// @brief Подставляет акцентные токены в шаблон QSS.
QString Fill(QString tpl, const QColor& acc, const QColor& accH, const QColor& accP, const QColor& soft,
             const QColor& selTxt, const QColor& onAcc)
{
    tpl.replace("@ACC@", acc.name());
    tpl.replace("@ACCH@", accH.name());
    tpl.replace("@ACCP@", accP.name());
    tpl.replace("@SOFT@", soft.name());
    tpl.replace("@SELTXT@", selTxt.name());
    tpl.replace("@ONACC@", onAcc.name());
    return tpl;
}

/// @brief Шаблон светлой темы (акцентные цвета — токены @…@).
const char* kLight = R"(
QMainWindow, QDialog, QWidget { background: #faf9f8; color: #2b2b30; font-size: 13px; }

#brandBar { background: #ffffff; border-bottom: 1px solid #ececeb; }
#brandPath { color: #8a8a92; font-size: 12px; }

QToolBar { background: #f3f2f1; border: none; border-bottom: 1px solid #e4e3e1; spacing: 8px; padding: 8px 14px; }
QToolBar::separator { background: #e4e3e1; width: 1px; margin: 6px 4px; }

#segmented { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 0; }
QToolButton#segButton { background: transparent; border: none; border-radius: 0; padding: 5px 10px; }
QToolButton#segButton:hover { background: #f0efed; }
QToolButton#segButton:checked { background: @ACC@; }

QPushButton#primaryButton { background: @ACC@; color: @ONACC@; border: none; border-radius: 0; padding: 6px 16px; font-weight: 600; }
QPushButton#primaryButton:hover { background: @ACCH@; }
QPushButton#primaryButton:pressed { background: @ACCP@; }

QPushButton#ghostButton { background: #ffffff; color: #2b2b30; border: 1px solid #e0dfdd; border-radius: 0; padding: 6px 16px; font-weight: 600; }
QPushButton#ghostButton:hover { border-color: @ACC@; color: @ACCH@; }

QToolButton#themeButton, QToolButton#paletteButton { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 0; padding: 6px 12px; color: #2b2b30; font-weight: 500; }
QToolButton#themeButton:hover, QToolButton#paletteButton:hover { border-color: @ACC@; color: @ACCH@; }
QToolButton#paletteButton::menu-indicator { image: none; width: 0; }

QCheckBox { spacing: 8px; color: #2b2b30; font-weight: 500; }

QTreeView { background: #f3f2f1; border: none; outline: 0; show-decoration-selected: 1; }
QTreeView::item { border-radius: 0; }
QTreeView::item:hover { background: #ebeae8; }
QTreeView::item:selected { background: @SOFT@; color: @SELTXT@; }
QTreeView::branch { background: transparent; }
QTreeView::branch:selected { background: @SOFT@; }
QHeaderView::section { background: #f3f2f1; border: none; color: #8a8a92; padding: 6px 4px; font-size: 11px; font-weight: 600; }

#toolbarSpacer { background: transparent; }

QMenu { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 0; padding: 6px; }
QLabel#popoverHeader { color: #8a8a92; font-size: 11px; font-weight: 600; }

QLabel#plotTitle { font-size: 16px; font-weight: 600; padding: 14px 0 8px; color: #2b2b30; letter-spacing: 0.01em; }
QChartView { background: #ffffff; border: none; }

QStatusBar { background: #f3f2f1; border-top: 1px solid #e4e3e1; color: #8a8a92; }
QStatusBar::item { border: none; }
QStatusBar QLabel { color: #8a8a92; }

QSplitter::handle { background: #e4e3e1; }
QSplitter::handle:hover { background: @ACC@; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 0; }
QScrollBar::handle:vertical { background: #d6d5d2; border-radius: 0; min-height: 24px; }
QScrollBar::handle:vertical:hover { background: @ACC@; }
QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
)";

/// @brief Шаблон тёмной темы (акцентные цвета — токены @…@).
const char* kDark = R"(
QMainWindow, QDialog, QWidget { background: #26262b; color: #ececee; font-size: 13px; }

#brandBar { background: #2e2e34; border-bottom: 1px solid #3a3a42; }
#brandPath { color: #9a9aa2; font-size: 12px; }

QToolBar { background: #2e2e34; border: none; border-bottom: 1px solid #3a3a42; spacing: 8px; padding: 8px 14px; }
QToolBar::separator { background: #3a3a42; width: 1px; margin: 6px 4px; }

#segmented { background: #34343b; border: 1px solid #44444e; border-radius: 0; }
QToolButton#segButton { background: transparent; border: none; border-radius: 0; padding: 5px 10px; }
QToolButton#segButton:hover { background: #3d3d45; }
QToolButton#segButton:checked { background: @ACC@; }

QPushButton#primaryButton { background: @ACC@; color: @ONACC@; border: none; border-radius: 0; padding: 6px 16px; font-weight: 600; }
QPushButton#primaryButton:hover { background: @ACCH@; }
QPushButton#primaryButton:pressed { background: @ACCP@; }

QPushButton#ghostButton { background: #34343b; color: #ececee; border: 1px solid #44444e; border-radius: 0; padding: 6px 16px; font-weight: 600; }
QPushButton#ghostButton:hover { border-color: @ACC@; color: @SELTXT@; }

QToolButton#themeButton, QToolButton#paletteButton { background: #34343b; border: 1px solid #44444e; border-radius: 0; padding: 6px 12px; color: #ececee; font-weight: 500; }
QToolButton#themeButton:hover, QToolButton#paletteButton:hover { border-color: @ACC@; color: @SELTXT@; }
QToolButton#paletteButton::menu-indicator { image: none; width: 0; }

QCheckBox { spacing: 8px; color: #ececee; font-weight: 500; }

QTreeView { background: #2e2e34; border: none; outline: 0; color: #ececee; show-decoration-selected: 1; }
QTreeView::item { border-radius: 0; }
QTreeView::item:hover { background: #383840; }
QTreeView::item:selected { background: @SOFT@; color: @SELTXT@; }
QTreeView::branch { background: transparent; }
QTreeView::branch:selected { background: @SOFT@; }
QHeaderView::section { background: #2e2e34; border: none; color: #9a9aa2; padding: 6px 4px; font-size: 11px; font-weight: 600; }

#toolbarSpacer { background: transparent; }

QMenu { background: #2e2e34; border: 1px solid #44444e; border-radius: 0; padding: 6px; }
QLabel#popoverHeader { color: #9a9aa2; font-size: 11px; font-weight: 600; }

QLabel#plotTitle { font-size: 16px; font-weight: 600; padding: 14px 0 8px; color: #ececee; letter-spacing: 0.01em; }
QChartView { background: #2e2e34; border: none; }

QStatusBar { background: #2e2e34; border-top: 1px solid #3a3a42; color: #9a9aa2; }
QStatusBar::item { border: none; }
QStatusBar QLabel { color: #9a9aa2; }

QSplitter::handle { background: #3a3a42; }
QSplitter::handle:hover { background: @ACC@; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 0; }
QScrollBar::handle:vertical { background: #44444e; border-radius: 0; min-height: 24px; }
QScrollBar::handle:vertical:hover { background: @ACC@; }
QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
)";

}  // namespace

QString StyleSheet(Mode mode, const QColor& accent)
{
    const QColor onAcc = Luma(accent) > 0.6 ? QColor(0x1a, 0x1a, 0x1e) : QColor(0xff, 0xff, 0xff);
    if (mode == Mode::Dark)
    {
        const QColor accH = accent.lighter(114);
        const QColor accP = accent.darker(115);
        const QColor soft = Mix(accent, QColor(0x2e, 0x2e, 0x34), 0.72);
        const QColor selTxt = Mix(accent, QColor(0xff, 0xff, 0xff), 0.5);
        return Fill(kDark, accent, accH, accP, soft, selTxt, onAcc);
    }
    const QColor accH = accent.darker(118);
    const QColor accP = accent.darker(142);
    const QColor soft = Mix(accent, QColor(0xff, 0xff, 0xff), 0.85);
    const QColor selTxt = accent.darker(142);
    return Fill(kLight, accent, accH, accP, soft, selTxt, onAcc);
}

}  // namespace gui::theme
