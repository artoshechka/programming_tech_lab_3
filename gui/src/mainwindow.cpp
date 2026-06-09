/// @file mainwindow.cpp
/// @brief Определение MainWindow
/// @author Artemenko Anton

#include "mainwindow.hpp"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <database_module/idatabase.hpp>
#include <logger/logger_macros.hpp>
#include <parser/parse_exception.hpp>

#include "chart_presenter.hpp"
#include "table_select_dialog.hpp"
#include "ui_strings.hpp"

QT_CHARTS_USE_NAMESPACE

namespace gui
{

/// @brief Виртуальный деструктор.
MainWindow::~MainWindow() = default;

/// @brief Конструктор: создаёт панель инструментов, дерево файлов и область графика, связывает сигналы.
MainWindow::MainWindow(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                       std::shared_ptr<logger::ILogger> logger, QWidget* parent)
    : QMainWindow(parent),
      registry_(registry),
      logger_(logger),
      presenter_(std::make_unique<ChartPresenter>(builders, styles, std::move(registry), logger_))
{
    auto* toolbar = addToolBar(ui::kToolbarTitle);
    toolbar->setMovable(true);

    chartCombo_ = new QComboBox();
    for (const auto& [name, _] : builders) chartCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(chartCombo_);
    toolbar->addSeparator();

    styleCombo_ = new QComboBox();
    for (const auto& [name, _] : styles) styleCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(styleCombo_);
    toolbar->addSeparator();

    aggregateCheck_ = new QCheckBox(ui::kAggregateCheckbox);
    aggregateCheck_->setChecked(true);
    aggregateCheck_->setEnabled(chartCombo_->currentText() == "Pie");
    toolbar->addWidget(aggregateCheck_);
    toolbar->addSeparator();

    auto* folderBtn = new QPushButton(ui::kFolderButton);
    auto* pdfBtn = new QPushButton(ui::kSavePdfButton);
    toolbar->addWidget(folderBtn);
    toolbar->addWidget(pdfBtn);

    treeView_ = new QTreeView();
    chartView_ = new QChartView();
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setChart(new QChart());

    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(treeView_);
    splitter->addWidget(chartView_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    setCentralWidget(splitter);

    const QString root = QFileDialog::getExistingDirectory(this, ui::kChooseFolderTitle);
    if (!root.isEmpty()) setRoot(root);

    connect(treeView_, &QTreeView::clicked, this, &MainWindow::onFileSelected);
    connect(folderBtn, &QPushButton::clicked, this, &MainWindow::onChooseFolder);
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::onSavePdf);
    connect(chartCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this]
            {
                aggregateCheck_->setEnabled(chartCombo_->currentText() == "Pie");
                onRedraw();
            });
    connect(styleCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onRedraw);
    connect(aggregateCheck_, &QCheckBox::stateChanged, this, &MainWindow::onRedraw);
}

/// @brief Устанавливает корневую папку дерева файлов.
void MainWindow::setRoot(const QString& path)
{
    auto* old = qobject_cast<QFileSystemModel*>(treeView_->model());
    auto* model = new QFileSystemModel(this);
    model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    QStringList filters;
    if (registry_)
        for (const auto& ext : registry_->SupportedExtensions())
            filters << QString::fromStdString("*." + ext);
    model->setNameFilters(filters);
    model->setNameFilterDisables(false);
    model->setRootPath(path);
    treeView_->setModel(model);
    treeView_->setRootIndex(model->index(path));
    treeView_->setColumnWidth(0, 220);
    for (int c = 1; c <= 3; ++c) treeView_->hideColumn(c);
    if (old) old->deleteLater();
    statusBar()->showMessage(path);
}

/// @brief Слот выбора рабочей папки с данными.
void MainWindow::onChooseFolder()
{
    const QString path = QFileDialog::getExistingDirectory(this, ui::kChooseFolderTitle);
    if (!path.isEmpty())
    {
        LogInfo(logger_) << "Working folder changed: " << path.toStdString();
        currentSource_.clear();
        setRoot(path);
    }
}

/// @brief Слот перерисовки графика при смене построителя или стиля.
void MainWindow::onRedraw()
{
    if (currentSource_.isEmpty()) return;
    try
    {
        auto chart = presenter_->rebuild(chartCombo_->currentText().toStdString(),
                                         styleCombo_->currentText().toStdString(), aggregateCheck_->isChecked());
        if (chart) setChart(std::move(chart));
    } catch (const std::exception& e)
    {
        LogError(logger_) << "Redraw failed: " << e.what();
        QMessageBox::critical(this, ui::kErrorTitle, QString::fromStdString(e.what()));
    }
}

/// @brief Слот выбора файла в дереве.
void MainWindow::onFileSelected(const QModelIndex& index)
{
    auto* model = static_cast<QFileSystemModel*>(treeView_->model());
    if (model->isDir(index)) return;
    const QString path = model->filePath(index);
    LogInfo(logger_) << "File selected: " << path.toStdString();
    loadFile(path);
}

/// @brief Загружает источник данных и строит график.
void MainWindow::loadFile(const QString& path)
{
    std::string source = path.toStdString();

    // Если формат имеет несколько под-источников (таблицы SQLite и т.п.) — спросить у пользователя.
    // Инспекция делегирована презентеру/парсеру; здесь остаётся только диалог, без знания формата.
    const auto subSources = presenter_->listSubSources(source);
    if (subSources.size() > 1)
    {
        QStringList qTables;
        for (const auto& t : subSources) qTables << QString::fromStdString(t);
        TableSelectDialog dlg(qTables, this);
        if (dlg.exec() != QDialog::Accepted) return;
        const QString selected = dlg.selectedTable();
        if (selected.isEmpty()) return;
        LogInfo(logger_) << "Sub-source selected: " << selected.toStdString();
        source += "|" + selected.toStdString();
    }

    try
    {
        currentSource_ = QString::fromStdString(source);
        auto chart = presenter_->load(source, chartCombo_->currentText().toStdString(),
                                      styleCombo_->currentText().toStdString(), aggregateCheck_->isChecked());
        setChart(std::move(chart));
        statusBar()->showMessage(path);
    } catch (const parser::ParseException& e)
    {
        LogError(logger_) << "Parse failed for '" << source << "': " << e.what();
        currentSource_.clear();
        QMessageBox::critical(this, ui::kLoadErrorTitle, QString::fromStdString(e.what()));
    } catch (const std::exception& e)
    {
        LogError(logger_) << "Load failed for '" << source << "': " << e.what();
        currentSource_.clear();
        QMessageBox::critical(this, ui::kErrorTitle, QString::fromStdString(e.what()));
    }
}

/// @brief Заменяет текущий график в области отображения.
void MainWindow::setChart(std::unique_ptr<QChart> chart)
{
    auto* old = chartView_->chart();
    // QChartView::setChart забирает владение голым указателем — единственный release()
    // в коде, на границе с Qt API.
    chartView_->setChart(chart.release());
    delete old;
}

/// @brief Слот сохранения текущего графика в PDF.
void MainWindow::onSavePdf()
{
    const QString path = QFileDialog::getSaveFileName(this, ui::kSavePdfDialogTitle, {}, ui::kPdfFilter);
    if (path.isEmpty()) return;
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);
    chartView_->render(&painter);
    painter.end();
    LogInfo(logger_) << "Chart saved to PDF: " << path.toStdString();
    statusBar()->showMessage(ui::kSavedPrefix + path);
}

}  // namespace gui
