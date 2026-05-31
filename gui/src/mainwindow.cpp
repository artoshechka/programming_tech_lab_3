#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QPdfWriter>
#include <QPainter>
#include <QVBoxLayout>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

namespace gui {

MainWindow::MainWindow(
    std::shared_ptr<chart::IChartBuilder>    builder,
    std::shared_ptr<style::IChartStyle>      style,
    std::shared_ptr<parser::IParserRegistry> registry,
    QWidget* parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , builder_(std::move(builder))
    , style_(std::move(style))
    , registry_(std::move(registry))
{
    ui_->setupUi(this);

    chartView_ = new QChartView(this);
    chartView_->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(ui_->chartContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView_);

    connect(ui_->openButton,   &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(ui_->savePdfButton, &QPushButton::clicked, this, &MainWindow::onSavePdf);
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::onOpenFile() {
    const QString path = QFileDialog::getOpenFileName(
        this, "Открыть файл данных", {},
        "Данные (*.sqlite *.json);;Все файлы (*)"
    );
    if (path.isEmpty()) return;
    loadFile(path);
}

void MainWindow::loadFile(const QString& path) {
    const std::string ext = QFileInfo(path).suffix().toLower().toStdString();
    auto parser = registry_->Get(ext);
    if (!parser) {
        statusBar()->showMessage("Неизвестный формат: " + QString::fromStdString(ext));
        return;
    }

    const data::TimelineData data = parser->Load(path.toStdString());
    QChart* chart = builder_->Build(data);
    style_->Apply(chart);

    chartView_->setChart(chart);
    ui_->savePdfButton->setEnabled(true);
    statusBar()->showMessage(path);
}

void MainWindow::onSavePdf() {
    const QString path = QFileDialog::getSaveFileName(
        this, "Сохранить PDF", {}, "PDF (*.pdf)"
    );
    if (path.isEmpty()) return;

    QPdfWriter writer(path);
    writer.setPageSize(QPagedPaintDevice::A4);
    QPainter painter(&writer);
    chartView_->render(&painter);
    painter.end();
    statusBar()->showMessage("Сохранено: " + path);
}

} // namespace gui
