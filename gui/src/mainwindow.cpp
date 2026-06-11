/// @file mainwindow.cpp
/// @brief Определение MainWindow
/// @author Artemenko Anton

#include <gui/mainwindow.hpp>

#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMargins>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLegend>
#include <chart/ichart_builder.hpp>
#include <logger/logger_macros.hpp>

#include "chart_model.hpp"
#include "table_select_dialog.hpp"
#include <gui/src/theme.hpp>
#include <gui/src/toggle_switch.hpp>
#include <gui/ui_strings.hpp>

QT_CHARTS_USE_NAMESPACE

namespace gui
{

MainWindow::~MainWindow() = default;

/// @brief Строит UI: тулбар с комбобоксами и кнопками, дерево файлов, область графика; связывает сигналы.
MainWindow::MainWindow(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                       const std::shared_ptr<logger::ILogger>& logger, QWidget* parent)
    : QMainWindow(parent),
      builders_(std::move(builders)),
      styles_(std::move(styles)),
      registry_(registry),
      logger_(logger),
      model_(std::make_unique<ChartModel>(std::move(registry), logger_))
{
    auto* toolbar = addToolBar(ui::kToolbarTitle);
    toolbar->setMovable(true);

    chartCombo_ = new QComboBox();
    for (const auto& [name, _] : builders_) chartCombo_->addItem(QString::fromStdString(name));
    toolbar->addWidget(chartCombo_);
    toolbar->addSeparator();

    // Палитра: кнопка со свотчем открывает поповер выбора палитры.
    buildPalettePopover();
    paletteButton_ = new QToolButton();
    paletteButton_->setObjectName("paletteButton");
    paletteButton_->setText(ui::kPaletteButton);
    paletteButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    paletteButton_->setPopupMode(QToolButton::InstantPopup);
    paletteButton_->setMenu(paletteMenu_);
    toolbar->addWidget(paletteButton_);
    toolbar->addSeparator();

    // Агрегация: ползунок-переключатель с подписью.
    aggregateSwitch_ = new ToggleSwitch();
    aggregateSwitch_->setChecked(true);
    aggregateSwitch_->setEnabled(chartCombo_->currentText() == "Pie");
    auto* aggWrap = new QWidget();
    auto* aggRow = new QHBoxLayout(aggWrap);
    aggRow->setContentsMargins(4, 0, 4, 0);
    aggRow->setSpacing(8);
    aggRow->addWidget(aggregateSwitch_);
    aggRow->addWidget(new QLabel(ui::kAggregateCheckbox));
    toolbar->addWidget(aggWrap);
    toolbar->addSeparator();

    // Распорка прижимает правую группу кнопок к краю тулбара (как в референс-дизайне).
    auto* spacer = new QWidget();
    spacer->setObjectName("toolbarSpacer");
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    auto* folderBtn = new QPushButton(ui::kFolderButton);
    folderBtn->setObjectName("primaryButton");
    auto* pdfBtn = new QPushButton(ui::kSavePdfButton);
    pdfBtn->setObjectName("ghostButton");
    themeButton_ = new QToolButton();
    themeButton_->setObjectName("themeButton");
    themeButton_->setText(ui::kThemeDarkButton);
    toolbar->addWidget(folderBtn);
    toolbar->addWidget(pdfBtn);
    toolbar->addWidget(themeButton_);

    treeView_ = new QTreeView();
    treeView_->setHeaderHidden(true);
    chartView_ = new QChartView();
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setChart(new QChart());

    // Контейнер графика: заголовок ряда над областью графика.
    auto* plotContainer = new QWidget();
    auto* plotLayout = new QVBoxLayout(plotContainer);
    plotLayout->setContentsMargins(0, 0, 0, 0);
    plotLayout->setSpacing(0);
    plotTitle_ = new QLabel(ui::kPlotTitlePlaceholder);
    plotTitle_->setObjectName("plotTitle");
    plotTitle_->setAlignment(Qt::AlignCenter);
    plotLayout->addWidget(plotTitle_);
    plotLayout->addWidget(chartView_, 1);

    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(treeView_);
    splitter->addWidget(plotContainer);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    setCentralWidget(splitter);

    statusInfo_ = new QLabel();
    statusBar()->addPermanentWidget(statusInfo_);
    qApp->setStyleSheet(theme::StyleSheet(theme::Mode::Light));

    // Начальная синхронизация модели с виджетами до подключения сигналов: модель должна знать
    // builder/style/aggregate ещё до первой загрузки источника.
    model_->setBuilder(chartCombo_->currentText().toStdString());
    std::string initialStyle = ui::kDefaultStyleName;
    if (styles_.find(initialStyle) == styles_.end() && !styles_.empty()) initialStyle = styles_.begin()->first;
    if (auto it = styles_.find(initialStyle); it != styles_.end())
    {
        model_->setStyle(initialStyle);
        updatePaletteButton(it->second()->ColorFor(0, 8));
    }
    model_->setAggregate(aggregateSwitch_->isChecked());

    const QString root = QFileDialog::getExistingDirectory(this, ui::kChooseFolderTitle);
    if (!root.isEmpty()) setRoot(root);

    // Сигналы модели -> представление: модель уведомляет, View перестраивает график / показывает ошибку.
    connect(model_.get(), &ChartModel::dataChanged, this, &MainWindow::refresh);
    connect(model_.get(), &ChartModel::renderOptionsChanged, this, &MainWindow::refresh);
    connect(model_.get(), &ChartModel::errorOccurred, this, &MainWindow::onError);

    // Сигналы виджетов -> слоты-мутаторы модели (роль контроллера в Qt Model/View).
    connect(treeView_, &QTreeView::clicked, this, &MainWindow::onFileSelected);
    connect(folderBtn, &QPushButton::clicked, this, &MainWindow::onChooseFolder);
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::onSavePdf);
    connect(themeButton_, &QToolButton::clicked, this, &MainWindow::toggleTheme);
    connect(chartCombo_, &QComboBox::currentTextChanged, this, [this](const QString& name) {
        aggregateSwitch_->setEnabled(name == "Pie");
        model_->setBuilder(name.toStdString());
    });
    connect(aggregateSwitch_, &ToggleSwitch::toggled, this, [this](bool on) { model_->setAggregate(on); });
}

/// @brief Пересоздаёт QFileSystemModel с фильтрами по реестру парсеров и задаёт корень дерева.
void MainWindow::setRoot(const QString& path)
{
    auto* old = qobject_cast<QFileSystemModel*>(treeView_->model());
    auto* model = new QFileSystemModel(this);
    model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    QStringList filters;
    if (registry_)
        for (const auto& ext : registry_->SupportedExtensions()) filters << QString::fromStdString("*." + ext);
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

/// @brief Открывает диалог выбора папки; обновляет дерево.
void MainWindow::onChooseFolder()
{
    const QString path = QFileDialog::getExistingDirectory(this, ui::kChooseFolderTitle);
    if (!path.isEmpty())
    {
        LogInfo(logger_) << "Working folder changed: " << path.toStdString();
        setRoot(path);
    }
}

/// @brief Перестраивает график из текущего состояния модели (реакция на сигналы модели).
void MainWindow::refresh()
{
    if (!model_->hasData()) return;  // данные ещё не загружены — строить нечего
    const auto& data = model_->data();
    setChart(buildChart(data));
    plotTitle_->setText(QString::fromStdString(data.name_));
    statusInfo_->setText(QString::number(static_cast<qulonglong>(data.points_.size())) + ui::kPointsSuffix);
}

/// @brief Переключает светлую/тёмную тему оформления и обновляет подпись кнопки.
void MainWindow::toggleTheme()
{
    darkTheme_ = !darkTheme_;
    qApp->setStyleSheet(theme::StyleSheet(darkTheme_ ? theme::Mode::Dark : theme::Mode::Light));
    themeButton_->setText(darkTheme_ ? ui::kThemeLightButton : ui::kThemeDarkButton);
    applyChartTheme(chartView_->chart());
    LogInfo(logger_) << "Theme switched to " << (darkTheme_ ? "dark" : "light");
}

/// @brief Строит выпадающий поповер выбора палитры со свотчами по доступным стилям.
void MainWindow::buildPalettePopover()
{
    paletteMenu_ = new QMenu(this);
    auto* popup = new QWidget(paletteMenu_);
    auto* col = new QVBoxLayout(popup);
    col->setContentsMargins(12, 12, 12, 12);
    col->setSpacing(10);

    auto* header = new QLabel(ui::kPaletteHeader);
    header->setObjectName("popoverHeader");
    col->addWidget(header);

    auto* grid = new QGridLayout();
    grid->setSpacing(8);
    int idx = 0;
    for (const auto& [name, factory] : styles_)
    {
        const QColor color = factory()->ColorFor(0, 8);
        auto* sw = new QToolButton(popup);
        sw->setObjectName("swatch");
        sw->setFixedSize(30, 30);
        sw->setToolTip(QString::fromStdString(name));
        sw->setStyleSheet(QStringLiteral("QToolButton#swatch{background:%1;border-radius:8px;border:2px solid transparent;}"
                                         "QToolButton#swatch:hover{border:2px solid #c0281a;}")
                              .arg(color.name()));
        const std::string styleName = name;
        connect(sw, &QToolButton::clicked, this, [this, styleName, color] {
            model_->setStyle(styleName);
            updatePaletteButton(color);
            paletteMenu_->hide();
        });
        grid->addWidget(sw, idx / 3, idx % 3);
        ++idx;
    }
    col->addLayout(grid);

    auto* action = new QWidgetAction(paletteMenu_);
    action->setDefaultWidget(popup);
    paletteMenu_->addAction(action);
}

/// @brief Обновляет цветной свотч на кнопке палитры.
void MainWindow::updatePaletteButton(const QColor& color)
{
    QPixmap pm(14, 14);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawRoundedRect(0, 0, 14, 14, 4, 4);
    p.end();
    paletteButton_->setIcon(QIcon(pm));
}

/// @brief Согласует оформление самого графика с активной темой приложения.
void MainWindow::applyChartTheme(QChart* chart)
{
    if (chart == nullptr) return;
    chart->setTitle({});  // имя ряда показывает plotTitle_, чтобы не дублировать заголовок
    chart->setBackgroundRoundness(12);
    chart->setMargins(QMargins(8, 8, 8, 8));

    const QColor bg = darkTheme_ ? QColor(0x2e, 0x2e, 0x34) : QColor(0xff, 0xff, 0xff);
    const QColor txt = darkTheme_ ? QColor(0xec, 0xec, 0xee) : QColor(0x2b, 0x2b, 0x30);
    const QColor grid = darkTheme_ ? QColor(0x3a, 0x3a, 0x42) : QColor(0xe4, 0xe3, 0xe1);

    chart->setBackgroundBrush(bg);
    chart->setBackgroundPen(Qt::NoPen);
    chart->setPlotAreaBackgroundVisible(false);
    if (chart->legend() != nullptr) chart->legend()->setLabelColor(txt);
    for (auto* axis : chart->axes())
    {
        axis->setLabelsColor(txt);
        axis->setTitleBrush(txt);
        axis->setLinePenColor(grid);
        axis->setGridLineColor(grid);
    }
}

/// @brief Показывает пользователю ошибку, пришедшую сигналом модели.
void MainWindow::onError(const QString& message)
{
    QMessageBox::critical(this, ui::kLoadErrorTitle, message);
}

/// @brief Обрабатывает клик по элементу дерева: пропускает папки, загружает файл.
void MainWindow::onFileSelected(const QModelIndex& index)
{
    auto* model = static_cast<QFileSystemModel*>(treeView_->model());
    if (model->isDir(index)) return;
    const QString path = model->filePath(index);
    LogInfo(logger_) << "File selected: " << path.toStdString();
    loadFile(path);
}

/// @brief Запрашивает под-источники, при необходимости показывает диалог выбора, затем задаёт источник модели.
void MainWindow::loadFile(const QString& path)
{
    std::string source = path.toStdString();

    // Если формат имеет несколько под-источников (таблицы SQLite и т.п.) — спросить у пользователя.
    // Инспекция делегирована модели/парсеру; здесь остаётся только диалог, без знания формата.
    const auto subSources = model_->listSubSources(source);
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

    // Мутация модели; дальше всё идёт через сигналы: dataChanged() -> refresh(), errorOccurred() -> onError().
    statusBar()->showMessage(path);
    model_->setSource(source);
}

/// @brief Строит QChart из данных по текущим builder/style/aggregate модели.
std::unique_ptr<QChart> MainWindow::buildChart(const data::TimelineData& data)
{
    auto chartStyle = styles_.at(model_->style())();
    auto chartBuilder = builders_.at(model_->builder())();
    chartBuilder->Configure(chart::BuilderOptions{model_->aggregate(), chartStyle.get()});
    std::unique_ptr<QChart> chart = chartBuilder->Build(data);
    chartStyle->Apply(chart.get());  // общие настройки графика (тема/фон)
    return chart;
}

/// @brief Передаёт новый график в QChartView, удаляя предыдущий.
void MainWindow::setChart(std::unique_ptr<QChart> chart)
{
    applyChartTheme(chart.get());
    auto* old = chartView_->chart();
    // QChartView::setChart забирает владение голым указателем — единственный release()
    // в коде, на границе с Qt API.
    chartView_->setChart(chart.release());
    delete old;
}

/// @brief Открывает диалог сохранения, рендерит текущий график в PDF через QPdfWriter.
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
