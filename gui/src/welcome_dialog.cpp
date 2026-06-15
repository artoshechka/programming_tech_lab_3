/// @file welcome_dialog.cpp
/// @brief Реализация стартового диалога-приглашения выбрать рабочую папку
/// @author Artemenko Anton

#include <QFileDialog>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <gui/src/welcome_dialog.hpp>
#include <gui/ui_strings.hpp>

namespace gui
{

WelcomeDialog::WelcomeDialog(const QColor& accent, QWidget* parent) : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8(ui::kWelcomeTitle));
    setModal(true);
    setMinimumWidth(440);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 36, 40, 36);
    layout->setSpacing(14);

    auto* heading = new QLabel(QString::fromUtf8(ui::kWelcomeHeading), this);
    QFont headingFont = heading->font();
    headingFont.setPointSize(headingFont.pointSize() + 7);
    headingFont.setBold(true);
    heading->setFont(headingFont);
    heading->setAlignment(Qt::AlignCenter);
    heading->setWordWrap(true);

    auto* subtitle = new QLabel(QString::fromUtf8(ui::kWelcomeSubtitle), this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("color: palette(mid);");

    auto* chooseButton = new QPushButton(QString::fromUtf8(ui::kWelcomeChooseButton), this);
    chooseButton->setCursor(Qt::PointingHandCursor);
    chooseButton->setMinimumHeight(40);
    chooseButton->setStyleSheet(QString("QPushButton { background-color: %1; color: white; border: none; "
                                        "border-radius: 8px; padding: 8px 20px; font-weight: bold; } "
                                        "QPushButton:hover { background-color: %2; }")
                                    .arg(accent.name(), accent.lighter(112).name()));

    layout->addWidget(heading);
    layout->addWidget(subtitle);
    layout->addSpacing(8);
    layout->addWidget(chooseButton);

    connect(chooseButton, &QPushButton::clicked, this, &WelcomeDialog::chooseFolder);
}

void WelcomeDialog::chooseFolder()
{
    const QString path = QFileDialog::getExistingDirectory(this, QString::fromUtf8(ui::kChooseFolderTitle));
    if (path.isEmpty()) return;
    selectedFolder_ = path;
    accept();
}

}  // namespace gui
