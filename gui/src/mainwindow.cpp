#include "mainwindow.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QPdfWriter>
#include <QPainter>
#include <QPushButton>
#include <QPageSize>

QT_CHARTS_USE_NAMESPACE

namespace gui {

MainWindow::MainWindow(
    BuilderFactory                           builders,
    StyleFactory                             styles,
    std::shared_ptr<parser::IParserRegistry> registry,
    QWidget* parent)
    : QMainWindow(parent)
    , builders_(std::move(builders))
    , styles_(std::move(styles))
    , registry_(std::move(registry))
{
    // --- тулбар ---
    auto* toolbar = addToolBar("Controls");
    toolbar->setMovable(false);

    chartCombo_ = new QComboBox();
    for (const auto& [name, _] : builders_) chartCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(chartCombo_);
    toolbar->addSeparator();

    styleCombo_ = new QComboBox();
    for (const auto& [name, _] : styles_) styleCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(styleCombo_);
    toolbar->addSeparator();

    auto* pdfBtn = new QPushButton("Сохранить PDF");
    toolbar->addWidget(pdfBtn);

    // --- дерево файлов ---
    const QString root = QFileDialog::getExistingDirectory(this, "Выберите папку с данными");

    auto* fsModel = new QFileSystemModel(this);
    fsModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    fsModel->setNameFilters({"*.sqlite", "*.json"});
    fsModel->setNameFilterDisables(false);
    fsModel->setRootPath(root);

    treeView_ = new QTreeView();
    treeView_->setModel(fsModel);
    treeView_->setRootIndex(fsModel->index(root));
    treeView_->setColumnWidth(0, 220);
    treeView_->hideColumn(1); // size
    treeView_->hideColumn(2); // type
    treeView_->hideColumn(3); // date

    // --- чарт ---
    chartView_ = new QChartView();
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setChart(new QChart());

    // --- сплиттер ---
    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(treeView_);
    splitter->addWidget(chartView_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    setCentralWidget(splitter);

    connect(treeView_,  &QTreeView::clicked,                  this, &MainWindow::onFileSelected);
    connect(pdfBtn,     &QPushButton::clicked,                this, &MainWindow::onSavePdf);
    connect(chartCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onRedraw);
    connect(styleCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onRedraw);
}

void MainWindow::onRedraw() {
    if (!currentPath_.isEmpty()) loadFile(currentPath_);
}

void MainWindow::onFileSelected(const QModelIndex& index) {
    auto* model = static_cast<QFileSystemModel*>(treeView_->model());
    if (model->isDir(index)) return;
    loadFile(model->filePath(index));
}

void MainWindow::loadFile(const QString& path) {
    currentPath_ = path;
    const std::string ext = QFileInfo(path).suffix().toLower().toStdString();
    auto parser = registry_->Get(ext);
    if (!parser) { statusBar()->showMessage("Неизвестный формат: " + QString::fromStdString(ext)); return; }

    const data::TimelineData data = parser->Load(path.toStdString());

    auto builder = builders_.at(chartCombo_->currentText().toStdString())();
    auto style   = styles_.at(styleCombo_->currentText().toStdString())();

    QChart* chart = builder->Build(data);
    style->Apply(chart);
    chartView_->setChart(chart);
    statusBar()->showMessage(path);
}

void MainWindow::onSavePdf() {
    const QString path = QFileDialog::getSaveFileName(this, "Сохранить PDF", {}, "PDF (*.pdf)");
    if (path.isEmpty()) return;
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);
    chartView_->render(&painter);
    painter.end();
    statusBar()->showMessage("Сохранено: " + path);
}

} // namespace gui
