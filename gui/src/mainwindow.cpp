/// @file mainwindow.cpp
/// @brief Определение MainWindow
/// @author Artemenko Anton

#include "mainwindow.hpp"

#include <QFileDialog>
#include <QFileInfo>
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
#include <parser/parse_exception.hpp>

#include "chart_presenter.hpp"
#include "table_select_dialog.hpp"

QT_CHARTS_USE_NAMESPACE

namespace gui
{

/// @brief Виртуальный деструктор.
MainWindow::~MainWindow() = default;

/// @brief Конструктор: создаёт панель инструментов, дерево файлов и область графика, связывает сигналы.
MainWindow::MainWindow(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                       std::shared_ptr<database::manager::IDatabaseManager> dbManager, QWidget* parent)
    : QMainWindow(parent),
      presenter_(std::make_unique<ChartPresenter>(builders, styles, registry, std::move(dbManager)))
{
    auto* toolbar = addToolBar("Controls");
    toolbar->setMovable(true);

    chartCombo_ = new QComboBox();
    for (const auto& [name, _] : builders) chartCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(chartCombo_);
    toolbar->addSeparator();

    styleCombo_ = new QComboBox();
    for (const auto& [name, _] : styles) styleCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(styleCombo_);
    toolbar->addSeparator();

    aggregateCheck_ = new QCheckBox("Агрегация");
    aggregateCheck_->setChecked(false);
    aggregateCheck_->setEnabled(chartCombo_->currentText() == "Pie");
    toolbar->addWidget(aggregateCheck_);
    toolbar->addSeparator();

    auto* folderBtn = new QPushButton("Папка");
    auto* pdfBtn = new QPushButton("Сохранить PDF");
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

    const QString root = QFileDialog::getExistingDirectory(this, "Выберите папку с данными");
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
    model->setNameFilters({"*.sqlite", "*.json"});
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
    const QString path = QFileDialog::getExistingDirectory(this, "Выберите папку с данными");
    if (!path.isEmpty())
    {
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
        QChart* chart = presenter_->rebuild(chartCombo_->currentText().toStdString(),
                                            styleCombo_->currentText().toStdString(), aggregateCheck_->isChecked());
        if (chart) setChart(chart);
    } catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}

/// @brief Слот выбора файла в дереве.
void MainWindow::onFileSelected(const QModelIndex& index)
{
    auto* model = static_cast<QFileSystemModel*>(treeView_->model());
    if (model->isDir(index)) return;
    loadFile(model->filePath(index));
}

/// @brief Загружает источник данных и строит график.
void MainWindow::loadFile(const QString& path)
{
    std::string source = path.toStdString();

    // Для SQLite с несколькими таблицами — спросить у пользователя.
    // Инспекция источника делегирована презентеру; здесь остаётся только диалог.
    if (QFileInfo(path).suffix().toLower() == "sqlite")
    {
        const auto tables = presenter_->listTables(source);
        if (tables.size() > 1)
        {
            QStringList qTables;
            for (const auto& t : tables) qTables << QString::fromStdString(t);
            TableSelectDialog dlg(qTables, this);
            if (dlg.exec() != QDialog::Accepted) return;
            const QString selected = dlg.selectedTable();
            if (selected.isEmpty()) return;
            source += "|" + selected.toStdString();
        }
    }

    try
    {
        currentSource_ = QString::fromStdString(source);
        QChart* chart = presenter_->load(source, chartCombo_->currentText().toStdString(),
                                         styleCombo_->currentText().toStdString(), aggregateCheck_->isChecked());
        setChart(chart);
        statusBar()->showMessage(path);
    } catch (const parser::ParseException& e)
    {
        currentSource_.clear();
        QMessageBox::critical(this, "Ошибка загрузки", QString::fromStdString(e.what()));
    } catch (const std::exception& e)
    {
        currentSource_.clear();
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}

/// @brief Заменяет текущий график в области отображения.
void MainWindow::setChart(QChart* chart)
{
    auto* old = chartView_->chart();
    chartView_->setChart(chart);
    delete old;
}

/// @brief Слот сохранения текущего графика в PDF.
void MainWindow::onSavePdf()
{
    const QString path = QFileDialog::getSaveFileName(this, "Сохранить PDF", {}, "PDF (*.pdf)");
    if (path.isEmpty()) return;
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);
    chartView_->render(&painter);
    painter.end();
    statusBar()->showMessage("Сохранено: " + path);
}

}  // namespace gui
