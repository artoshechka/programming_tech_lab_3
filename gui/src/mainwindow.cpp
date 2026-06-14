/// @file mainwindow.cpp
/// @brief Определение MainWindow
/// @author Artemenko Anton

#include <QApplication>
#include <QButtonGroup>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMargins>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QStringList>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <chart/ichart_builder.hpp>
#include <gui/builder_names.hpp>
#include <gui/mainwindow.hpp>
#include <gui/src/file_item_delegate.hpp>
#include <gui/src/file_row_widget.hpp>
#include <gui/src/scrollable_chart_view.hpp>
#include <gui/src/theme.hpp>
#include <gui/ui_strings.hpp>
#include <logger/logger_macros.hpp>

#include "chart_model.hpp"
#include "table_select_dialog.hpp"

QT_CHARTS_USE_NAMESPACE

namespace gui
{

MainWindow::~MainWindow() = default;

/// @brief Строит UI: тулбар с комбобоксами и кнопками, дерево файлов, область графика; связывает сигналы.
MainWindow::MainWindow(BuilderFactory builders, StyleFactory styles, ExporterFactory exporters,
                       std::shared_ptr<parser::IParserRegistry> registry,
                       const std::shared_ptr<logger::ILogger>& logger, QWidget* parent)
    : QMainWindow(parent),
      builders_(std::move(builders)),
      styles_(std::move(styles)),
      exporters_(std::move(exporters)),
      registry_(registry),
      logger_(logger),
      model_(std::make_unique<ChartModel>(std::move(registry), logger_))
{
    // Верхняя строка: путь к текущему файлу по центру (отступ слева — под нативные кнопки macOS).
    auto* brandBar = new QWidget();
    brandBar->setObjectName("brandBar");
    auto* brandRow = new QHBoxLayout(brandBar);
    brandRow->setContentsMargins(80, 6, 16, 6);
    brandRow->addStretch();
    pathLabel_ = new QLabel();
    pathLabel_->setObjectName("brandPath");
    brandRow->addWidget(pathLabel_);
    brandRow->addStretch();
    setMenuWidget(brandBar);

    auto* toolbar = addToolBar(ui::kToolbarTitle);
    toolbar->setMovable(false);

    // Тип графика: сегмент-контрол с иконками (по одной кнопке на построитель).
    auto* segWrap = new QWidget();
    segWrap->setObjectName("segmented");
    auto* segRow = new QHBoxLayout(segWrap);
    segRow->setContentsMargins(3, 3, 3, 3);
    segRow->setSpacing(3);
    chartTypeGroup_ = new QButtonGroup(this);
    chartTypeGroup_->setExclusive(true);
    for (const auto& [name, _] : builders_)
    {
        auto* seg = new QToolButton();
        seg->setObjectName("segButton");
        seg->setCheckable(true);
        seg->setText(QString::fromStdString(name));
        seg->setProperty("builderName", QString::fromStdString(name));
        // Тултип объясняет, как формируется график этого типа (показывается при наведении).
        seg->setToolTip(QString::fromUtf8(ui::TooltipForBuilder(name)));
        segRow->addWidget(seg);
        chartTypeGroup_->addButton(seg);
    }
    toolbar->addWidget(segWrap);
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

    // Распорка прижимает правую группу кнопок к краю тулбара (как в референс-дизайне).
    auto* spacer = new QWidget();
    spacer->setObjectName("toolbarSpacer");
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    auto* folderBtn = new QPushButton(ui::kFolderButton);
    folderBtn->setObjectName("primaryButton");
    auto* saveBtn = new QPushButton(ui::kSaveButton);
    saveBtn->setObjectName("ghostButton");
    themeButton_ = new QToolButton();
    themeButton_->setObjectName("themeButton");
    themeButton_->setText(ui::kThemeDarkButton);
    toolbar->addWidget(folderBtn);
    toolbar->addWidget(saveBtn);
    toolbar->addWidget(themeButton_);

    treeView_ = new QTreeView();
    treeView_->setHeaderHidden(true);
    fileDelegate_ = new FileItemDelegate(this);
    treeView_->setItemDelegate(fileDelegate_);
    chartView_ = new ScrollableChartView();
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

    // Начальная синхронизация модели с виджетами до подключения сигналов: модель должна знать
    // builder/style ещё до первой загрузки источника. По умолчанию — линия времени ("Line").
    QString initialBuilder;
    if (!chartTypeGroup_->buttons().isEmpty())
    {
        QAbstractButton* defaultButton = chartTypeGroup_->buttons().first();
        for (auto* button : chartTypeGroup_->buttons())
            if (button->property("builderName").toString() == builders::kLine) defaultButton = button;
        defaultButton->setChecked(true);
        initialBuilder = defaultButton->property("builderName").toString();
    }
    model_->setBuilder(initialBuilder.toStdString());
    std::string initialStyle = ui::kDefaultStyleName;
    if (styles_.find(initialStyle) == styles_.end() && !styles_.empty()) initialStyle = styles_.begin()->first;
    if (auto it = styles_.find(initialStyle); it != styles_.end())
    {
        model_->setStyle(initialStyle);
        accent_ = it->second()->ColorFor(0, 8);
        updatePaletteButton(accent_);
    }

    const QString root = QFileDialog::getExistingDirectory(this, ui::kChooseFolderTitle);
    if (!root.isEmpty()) setRoot(root);

    // Сигналы модели -> представление: модель уведомляет, View перестраивает график / показывает ошибку.
    connect(model_.get(), &ChartModel::dataChanged, this, &MainWindow::refresh);
    connect(model_.get(), &ChartModel::renderOptionsChanged, this, &MainWindow::refresh);
    connect(model_.get(), &ChartModel::errorOccurred, this, &MainWindow::onError);

    // Сигналы виджетов -> слоты-мутаторы модели (роль контроллера в Qt Model/View).
    connect(treeView_, &QTreeView::clicked, this, &MainWindow::onFileSelected);
    connect(folderBtn, &QPushButton::clicked, this, &MainWindow::onChooseFolder);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveChart);
    connect(themeButton_, &QToolButton::clicked, this, &MainWindow::toggleTheme);
    connect(chartTypeGroup_, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this,
            [this](QAbstractButton* button) {
                model_->setBuilder(button->property("builderName").toString().toStdString());
            });
    // Раскрытие папки подгружает её содержимое — досоздаём живые виджеты строк.
    connect(treeView_, &QTreeView::expanded, this,
            [this](const QModelIndex&) { installRowWidgets(treeView_->rootIndex()); });

    applyTheme();  // первичное применение темы со стартовым акцентом
}

/// @brief Пересоздаёт QFileSystemModel (показывает все файлы и папки) и задаёт корень дерева.
/// @details Фильтр по расширению не накладывается: в дереве видны все файлы, тип каждого
///          различается иконкой (FileKind). Строить график умеют только поддерживаемые
///          форматы (json/sqlite) — выбор остальных файлов игнорируется в onFileSelected().
void MainWindow::setRoot(const QString& path)
{
    auto* old = qobject_cast<QFileSystemModel*>(treeView_->model());
    auto* model = new QFileSystemModel(this);
    model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    model->setRootPath(path);
    treeView_->setModel(model);
    treeView_->setRootIndex(model->index(path));
    treeView_->setColumnWidth(0, 220);
    for (int c = 1; c <= 3; ++c) treeView_->hideColumn(c);

    // Содержимое каталога загружается асинхронно: ставим живые виджеты строк по мере готовности
    // и при появлении новых строк, а выделение синхронизируем с моделью выбора.
    connect(model, &QFileSystemModel::directoryLoaded, this,
            [this](const QString&) { installRowWidgets(treeView_->rootIndex()); });
    connect(treeView_->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection&, const QItemSelection&) { updateRowSelection(); });
    installRowWidgets(treeView_->rootIndex());

    if (old) old->deleteLater();
    statusBar()->showMessage(path);
}

/// @brief Рекурсивно ставит живые виджеты строк (FileRowWidget) на видимые элементы дерева.
/// @details Создаёт виджет только там, где его ещё нет (идемпотентно), и спускается в раскрытые
///          папки. Вызывается при загрузке каталога, раскрытии папки и смене корня.
/// @param[in] parent Родительский индекс (поддерево для обхода).
void MainWindow::installRowWidgets(const QModelIndex& parent)
{
    auto* model = qobject_cast<QFileSystemModel*>(treeView_->model());
    if (model == nullptr) return;
    const int rows = model->rowCount(parent);
    for (int i = 0; i < rows; ++i)
    {
        const QModelIndex idx = model->index(i, 0, parent);
        if (treeView_->indexWidget(idx) == nullptr)
        {
            const bool isDir = model->isDir(idx);
            const QFileInfo info = model->fileInfo(idx);
            const FileKind kind = ClassifyFileKind(isDir, info.suffix().toLower());
            auto* rowWidget = new FileRowWidget(kind, isDir, idx.data(Qt::DisplayRole).toString(), info, idx);
            rowWidget->applyTheme(darkTheme_, accent_);
            treeView_->setIndexWidget(idx, rowWidget);
        }
        if (treeView_->isExpanded(idx)) installRowWidgets(idx);
    }
}

/// @brief Синхронизирует визуальное выделение живых строк с моделью выбора дерева.
void MainWindow::updateRowSelection()
{
    auto* selection = treeView_->selectionModel();
    for (auto* rowWidget : treeView_->findChildren<FileRowWidget*>())
        rowWidget->setSelected(selection != nullptr && selection->isSelected(rowWidget->index()));
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
    themeButton_->setText(darkTheme_ ? ui::kThemeLightButton : ui::kThemeDarkButton);
    applyTheme();
    LogInfo(logger_) << "Theme switched to " << (darkTheme_ ? "dark" : "light");
}

/// @brief Применяет тему ко всему приложению с текущим акцентом.
void MainWindow::applyTheme()
{
    qApp->setStyleSheet(theme::StyleSheet(darkTheme_ ? theme::Mode::Dark : theme::Mode::Light, accent_));
    for (auto* row : treeView_->findChildren<FileRowWidget*>()) row->applyTheme(darkTheme_, accent_);
    applyChartTheme(chartView_->chart());
    treeView_->viewport()->update();
    update();  // полная перерисовка всего окна под новую тему/акцент
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
        sw->setStyleSheet(QStringLiteral("QToolButton#swatch{background:%1;border:2px solid transparent;}"
                                         "QToolButton#swatch:hover{border:2px solid #c0281a;}")
                              .arg(color.name()));
        const std::string styleName = name;
        connect(sw, &QToolButton::clicked, this, [this, styleName, color] {
            accent_ = color;
            model_->setStyle(styleName);
            updatePaletteButton(color);
            applyTheme();  // перекрасить всё приложение под акцент палитры
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
    p.drawRect(0, 0, 14, 14);
    p.end();
    paletteButton_->setIcon(QIcon(pm));
}

/// @brief Согласует оформление самого графика с активной темой приложения.
void MainWindow::applyChartTheme(QChart* chart)
{
    if (chart == nullptr) return;
    chart->setTitle({});               // имя ряда показывает plotTitle_, чтобы не дублировать заголовок
    chart->setBackgroundRoundness(0);  // плоский фон без «карточки» — однородно с QChartView
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
    // Подписи секторов круговой диаграммы — под цвет темы (иначе тёмные на тёмном фоне).
    for (auto* series : chart->series())
    {
        if (auto* pie = qobject_cast<QtCharts::QPieSeries*>(series))
            for (auto* slice : pie->slices()) slice->setLabelColor(txt);
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

    // График строим только для поддерживаемых форматов (json/sqlite). Прочие файлы видны
    // в дереве со своей иконкой, но при выборе ничего не загружаем и не показываем ошибку.
    const std::string ext = QFileInfo(path).suffix().toLower().toStdString();
    bool supported = false;
    if (registry_)
        for (const auto& e : registry_->SupportedExtensions())
            if (e == ext)
            {
                supported = true;
                break;
            }
    if (!supported)
    {
        LogInfo(logger_) << "Unsupported file ignored: " << path.toStdString();
        return;
    }

    LogInfo(logger_) << "File selected: " << path.toStdString();
    loadFile(path);
}

/// @brief Запрашивает под-источники, при необходимости показывает диалог выбора, затем задаёт источник модели.
void MainWindow::loadFile(const QString& path)
{
    const QFileInfo info(path);
    pathLabel_->setText(info.dir().dirName() + " — " + info.fileName());
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
    // Новый график показываем вписанным в окно; масштаб/прокрутку пользователь задаёт сам.
    if (auto* scrollable = qobject_cast<ScrollableChartView*>(chartView_)) scrollable->resetView();
}

/// @brief Открывает диалог сохранения и экспортирует текущий график в выбранный формат.
void MainWindow::onSaveChart()
{
    if (exporters_.empty()) return;

    // Фильтр диалога и выбор экспортёра собираются из фабрики: слот не знает форматов.
    // Связь «строка фильтра -> расширение» строится здесь же, чтобы по выбору пользователя
    // (selectedFilter) найти нужный экспортёр без разбора литералов фильтров.
    QStringList filters;
    std::map<QString, std::string> filterToExt;
    for (const auto& [ext, make] : exporters_)
    {
        const QString filter = make()->FileFilter();
        filters << filter;
        filterToExt[filter] = ext;
    }

    QString selectedFilter;
    const QString path =
        QFileDialog::getSaveFileName(this, ui::kSaveChartDialogTitle, {}, filters.join(";;"), &selectedFilter);
    if (path.isEmpty()) return;

    // Расширение берём из выбранного фильтра; на случай нестандартного выбора — первый из фабрики.
    auto extIt = filterToExt.find(selectedFilter);
    const std::string ext = extIt != filterToExt.end() ? extIt->second : exporters_.begin()->first;
    const QString suffix = "." + QString::fromStdString(ext);
    QString outPath = path.endsWith(suffix, Qt::CaseInsensitive) ? path : path + suffix;

    auto* scene = chartView_->scene();
    if (scene == nullptr) return;
    if (!exporters_.at(ext)()->Export(*scene, outPath))
    {
        LogError(logger_) << "Chart export failed: " << outPath.toStdString();
        onError(ui::kSaveErrorMessage);
        return;
    }
    LogInfo(logger_) << "Chart saved: " << outPath.toStdString();
    statusBar()->showMessage(ui::kSavedPrefix + outPath);
}

}  // namespace gui
