/// @file theme.cpp
/// @brief Определение светлой и тёмной тем (QSS) с оранжевым акцентом.
/// @author Artemenko Anton
/// @details Палитра акцента (красно-оранжевый) и её оттенки:
///          base #c0281a, hover #a41f12, press #8c1a0f,
///          soft (свет) #f6ddd8, soft (тьма) #4d211a, dark-accent #d44530.

#include <gui/src/theme.hpp>

namespace gui::theme
{

namespace
{

/// @brief QSS светлой темы.
const QString kLight = R"(
QMainWindow, QDialog, QWidget { background: #faf9f8; color: #2b2b30; font-size: 13px; }

#brandBar { background: #ffffff; border-bottom: 1px solid #ececeb; }
#brandMark { background: #c0281a; border-radius: 5px; }
#brandName { font-weight: 700; font-size: 14px; color: #2b2b30; }
#brandPath { color: #8a8a92; font-size: 12px; }

QToolBar { background: #f3f2f1; border: none; border-bottom: 1px solid #e4e3e1; spacing: 8px; padding: 8px 14px; }
QToolBar::separator { background: #e4e3e1; width: 1px; margin: 6px 4px; }

QComboBox { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 8px; padding: 5px 12px; min-height: 24px; min-width: 70px; color: #2b2b30; combobox-popup: 0; }
QComboBox:hover { border-color: #c0281a; }
QComboBox::drop-down { border: none; width: 22px; }
QComboBox QAbstractItemView { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 10px;
    selection-background-color: #f6ddd8; selection-color: #8c1a0f; outline: 0; padding: 4px; }
QComboBox QAbstractItemView::item { min-height: 28px; padding: 4px 10px; border-radius: 7px; color: #2b2b30; }

#segmented { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 9px; }
QToolButton#segButton { background: transparent; border: none; border-radius: 6px; padding: 5px 10px; }
QToolButton#segButton:hover { background: #f0efed; }
QToolButton#segButton:checked { background: #c0281a; }

QPushButton#primaryButton { background: #c0281a; color: #ffffff; border: none; border-radius: 8px; padding: 6px 16px; font-weight: 600; }
QPushButton#primaryButton:hover { background: #a41f12; }
QPushButton#primaryButton:pressed { background: #8c1a0f; }

QPushButton#ghostButton { background: #ffffff; color: #2b2b30; border: 1px solid #e0dfdd; border-radius: 8px; padding: 6px 16px; font-weight: 600; }
QPushButton#ghostButton:hover { border-color: #c0281a; color: #a41f12; }

QToolButton#themeButton { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 8px; padding: 6px 12px; color: #2b2b30; }
QToolButton#themeButton:hover { border-color: #c0281a; color: #a41f12; }

QCheckBox { spacing: 8px; color: #2b2b30; font-weight: 500; }
QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #c9c8c6; border-radius: 5px; background: #ffffff; }
QCheckBox::indicator:checked { background: #c0281a; border-color: #c0281a; }
QCheckBox::indicator:disabled { background: #efeeec; border-color: #ddddda; }

QTreeView { background: #f3f2f1; border: none; outline: 0; show-decoration-selected: 1; }
QTreeView::item { border-radius: 7px; }
QTreeView::item:hover { background: #ebeae8; }
QTreeView::item:selected { background: #f6ddd8; color: #8c1a0f; }
QTreeView::branch { background: transparent; }
QTreeView::branch:selected { background: #f6ddd8; }
QHeaderView::section { background: #f3f2f1; border: none; color: #8a8a92; padding: 6px 4px; font-size: 11px; font-weight: 600; }

#toolbarSpacer { background: transparent; }

QToolButton#paletteButton { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 8px; padding: 6px 12px; color: #2b2b30; font-weight: 500; }
QToolButton#paletteButton:hover { border-color: #c0281a; color: #a41f12; }
QToolButton#paletteButton::menu-indicator { image: none; width: 0; }

QMenu { background: #ffffff; border: 1px solid #e0dfdd; border-radius: 12px; padding: 6px; }
QLabel#popoverHeader { color: #8a8a92; font-size: 11px; font-weight: 600; }

QLabel#plotTitle { font-size: 16px; font-weight: 600; padding: 14px 0 8px; color: #2b2b30; letter-spacing: 0.01em; }
QChartView { background: #ffffff; border: none; }

QStatusBar { background: #f3f2f1; border-top: 1px solid #e4e3e1; color: #8a8a92; }
QStatusBar::item { border: none; }
QStatusBar QLabel { color: #8a8a92; }

QSplitter::handle { background: #e4e3e1; }
QSplitter::handle:hover { background: #c0281a; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 0; }
QScrollBar::handle:vertical { background: #d6d5d2; border-radius: 5px; min-height: 24px; }
QScrollBar::handle:vertical:hover { background: #c0281a; }
QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
)";

/// @brief QSS тёмной темы.
const QString kDark = R"(
QMainWindow, QDialog, QWidget { background: #26262b; color: #ececee; font-size: 13px; }

#brandBar { background: #2e2e34; border-bottom: 1px solid #3a3a42; }
#brandMark { background: #d44530; border-radius: 5px; }
#brandName { font-weight: 700; font-size: 14px; color: #ececee; }
#brandPath { color: #9a9aa2; font-size: 12px; }

QToolBar { background: #2e2e34; border: none; border-bottom: 1px solid #3a3a42; spacing: 8px; padding: 8px 14px; }
QToolBar::separator { background: #3a3a42; width: 1px; margin: 6px 4px; }

QComboBox { background: #34343b; border: 1px solid #44444e; border-radius: 8px; padding: 5px 12px; min-height: 24px; min-width: 70px; color: #ececee; combobox-popup: 0; }
QComboBox:hover { border-color: #d44530; }
QComboBox::drop-down { border: none; width: 22px; }
QComboBox QAbstractItemView { background: #2e2e34; border: 1px solid #44444e; border-radius: 10px;
    selection-background-color: #4d211a; selection-color: #f5b6a8; outline: 0; padding: 4px; color: #ececee; }
QComboBox QAbstractItemView::item { min-height: 28px; padding: 4px 10px; border-radius: 7px; color: #ececee; }

#segmented { background: #34343b; border: 1px solid #44444e; border-radius: 9px; }
QToolButton#segButton { background: transparent; border: none; border-radius: 6px; padding: 5px 10px; }
QToolButton#segButton:hover { background: #3d3d45; }
QToolButton#segButton:checked { background: #d44530; }

QPushButton#primaryButton { background: #d44530; color: #1a1a1e; border: none; border-radius: 8px; padding: 6px 16px; font-weight: 600; }
QPushButton#primaryButton:hover { background: #e0563f; }
QPushButton#primaryButton:pressed { background: #b83925; }

QPushButton#ghostButton { background: #34343b; color: #ececee; border: 1px solid #44444e; border-radius: 8px; padding: 6px 16px; font-weight: 600; }
QPushButton#ghostButton:hover { border-color: #d44530; color: #f5b6a8; }

QToolButton#themeButton { background: #34343b; border: 1px solid #44444e; border-radius: 8px; padding: 6px 12px; color: #ececee; }
QToolButton#themeButton:hover { border-color: #d44530; color: #f5b6a8; }

QCheckBox { spacing: 8px; color: #ececee; font-weight: 500; }
QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #55555f; border-radius: 5px; background: #34343b; }
QCheckBox::indicator:checked { background: #d44530; border-color: #d44530; }
QCheckBox::indicator:disabled { background: #2e2e34; border-color: #3a3a42; }

QTreeView { background: #2e2e34; border: none; outline: 0; color: #ececee; show-decoration-selected: 1; }
QTreeView::item { border-radius: 7px; }
QTreeView::item:hover { background: #383840; }
QTreeView::item:selected { background: #4d211a; color: #f5b6a8; }
QTreeView::branch { background: transparent; }
QTreeView::branch:selected { background: #4d211a; }
QHeaderView::section { background: #2e2e34; border: none; color: #9a9aa2; padding: 6px 4px; font-size: 11px; font-weight: 600; }

#toolbarSpacer { background: transparent; }

QToolButton#paletteButton { background: #34343b; border: 1px solid #44444e; border-radius: 8px; padding: 6px 12px; color: #ececee; font-weight: 500; }
QToolButton#paletteButton:hover { border-color: #d44530; color: #f5b6a8; }
QToolButton#paletteButton::menu-indicator { image: none; width: 0; }

QMenu { background: #2e2e34; border: 1px solid #44444e; border-radius: 12px; padding: 6px; }
QLabel#popoverHeader { color: #9a9aa2; font-size: 11px; font-weight: 600; }

QLabel#plotTitle { font-size: 16px; font-weight: 600; padding: 14px 0 8px; color: #ececee; letter-spacing: 0.01em; }
QChartView { background: #2e2e34; border: none; }

QStatusBar { background: #2e2e34; border-top: 1px solid #3a3a42; color: #9a9aa2; }
QStatusBar::item { border: none; }
QStatusBar QLabel { color: #9a9aa2; }

QSplitter::handle { background: #3a3a42; }
QSplitter::handle:hover { background: #d44530; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 0; }
QScrollBar::handle:vertical { background: #44444e; border-radius: 5px; min-height: 24px; }
QScrollBar::handle:vertical:hover { background: #d44530; }
QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
)";

}  // namespace

QString StyleSheet(Mode mode)
{
    return mode == Mode::Dark ? kDark : kLight;
}

}  // namespace gui::theme
