# Лабораторная работа по предмету: "Разработка средств защиты информации"
## Тема: "QtWidgets - Печать графиков"
> 4 курс 2 семестр \
> Студент группы 932223 - **Артеменко Антон Дмитриевич**

## Постановка задачи

### Вспомогательная часть

В рамках лабораторной работы необходимо рассмотреть реализацию IoC-контейнера и
реализовать задание, рассмотренное на лекциях. Реализация IoC-контейнера в проекте —
[`ioc_container/IOC_Contaner.hpp`](ioc_container/IOC_Contaner.hpp).

### Основная часть

Разработка приложения печати графиков.

**Дано:** предложен начальный вариант архитектуры ПО, в которую требуется внести изменения
с целью снижения связности архитектуры. Используется принцип внедрения зависимости.
Реализация внедрения зависимости — с помощью IoC-контейнера.

**При разработке архитектуры учесть:**

- Возможность добавления новых графиков (графики отличаются видом и данными).
- Изменение визуального стиля графиков (цветной, чёрно-белый).

**Общие требования к GUI:**

- Загружаем данные путём выбора нужного файла. Данные в ПО не отображаем — отображаем
  только график, построенный относительно считанных данных.
- При печати в PDF выбираем место сохранения графика.

### Часть №1. Формат исходных данных. Варианты представления входных данных

Исходные данные для печати соответствуют некоторому типу, который определяется
пользователем. Данные определённого типа могут отображаться конкретным графиком,
ориентированным на этот тип данных.

**Примеры данных:**

1. Данные характеризуются парой `[дата, значение]`, хранятся в БД SQLite (архив с данными
   прилагается). Информация по организации работы с БД SQLite:
   <https://habr.com/ru/post/128836/>
2. Данные представлены JSON-файлом (примеры сгенерировать самостоятельно). Формат данных
   `[дата, значение]`. Информация для знакомства и взаимодействия с JSON:
   - <https://habr.com/ru/articles/554274/>
   - <https://doc.qt.io/qt-5/json.html>
   - <https://tproger.ru/articles/chto-takoe-json-vvedenie/>

## Предлагаемое решение

### Зависимости проекта

В проекте используется:

- **CMake** ≥ 3.16
- **Стандарт C++** 17
- **Qt 5** — компоненты `Widgets`, `Charts`, `PrintSupport`, `Core`, `Sql`

`AUTOMOC`/`AUTOUIC` включены — отдельный запуск `moc`/`uic` не требуется.

### Сборка (CMake)

```bash
mkdir -p build && cd buid

cmake .. && cmake --build .

./programming_tech_lab_3
```

## Документация (UML)

Диаграмма классов разбита по модулям — так стрелки не пересекаются и каждый блок
читается отдельно. Исходники — в [`docs/pics`](docs/pics). Внутри модульных диаграмм
коллабораторы из других модулей показаны заглушками со стереотипом-источником
(например, `<<logger>>`) — без членов.

### Обзор (связи между модулями)

Ключевые сущности каждого модуля без членов и связи между ними.
Исходник: [`docs/pics/class_overview.mmd`](docs/pics/class_overview.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction LR

    class ILogger {
        <<logger>>
    }
    class TimelineData {
        <<data_model>>
    }
    class IParser {
        <<parser>>
    }
    class IParserRegistry {
        <<parser>>
    }
    class IDatabase {
        <<database_module>>
    }
    class IDatabaseManager {
        <<database_module>>
    }
    class IChartBuilder {
        <<chart>>
    }
    class BuilderOptions {
        <<chart>>
    }
    class IPalette {
        <<style>>
    }
    class IChartStyle {
        <<style>>
    }
    class IChartExporter {
        <<gui>>
    }
    class ChartModel {
        <<gui>>
    }
    class MainWindow {
        <<gui>>
    }
    class IOCContainer {
        <<ioc>>
    }

    IOCContainer ..> ILogger : wires
    IOCContainer ..> IDatabaseManager : wires
    IOCContainer ..> IParserRegistry : wires

    IParserRegistry o-- IParser
    IParser ..> TimelineData
    IParser ..> IDatabaseManager
    IParser ..> ILogger

    IDatabaseManager ..> IDatabase : creates
    IDatabase ..> ILogger

    IChartBuilder ..> TimelineData
    IChartBuilder ..> BuilderOptions
    IChartBuilder ..> IPalette
    IChartBuilder ..> ILogger
    BuilderOptions o-- IPalette

    IChartStyle --|> IPalette
    IChartStyle ..> ILogger

    MainWindow *-- ChartModel
    MainWindow o-- IChartBuilder
    MainWindow o-- IChartStyle
    MainWindow o-- IChartExporter
    MainWindow o-- IParserRegistry
    MainWindow o-- ILogger
    ChartModel o-- IParserRegistry
    ChartModel *-- TimelineData
    ChartModel o-- ILogger
```

### Модуль logger

Исходник: [`docs/pics/class_logger.mmd`](docs/pics/class_logger.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class ILogger {
        <<interface>>
        +~ILogger() virtual
        +SetSettings(settings) void
        +GetSettings() LoggerSettings
        +Log(level, message, file, line, function) void
    }
    class ThreadSafeLogger {
        +ThreadSafeLogger(componentName, output)
        +~ThreadSafeLogger()
        +SetSettings(settings) void
        +GetSettings() LoggerSettings
        +Log(level, message, file, line, function) void
        #FormatMessage(level, message, file, line, function) string
        #LogLevelToString(level) string$
        #string componentName_
        -OpenLogFile() void
        -ofstream logFile_
        -recursive_mutex syncMutex_
        -LoggerSettings settings_
    }
    class AppLogger {
        +AppLogger(output)
        +~AppLogger()
        -FormatMessage(level, message, file, line, function) string
    }
    class LogEntryStream {
        +LogEntryStream(logger, level, file, line, function)
        +~LogEntryStream()
        +operator<<(value) LogEntryStream&
        -shared_ptr~ILogger~ logger_
        -LogLevel level_
        -const char* file_
        -int line_
        -const char* function_
        -ostringstream stream_
    }
    class LoggerSettings {
        <<struct>>
        +LoggerSettings(logFilePath, logLevel, output)
        +optional~string~ logFilePath_
        +LogLevel logLevel_
        +LogOutput output_
    }
    class LogLevel {
        <<enumeration>>
        Trace
        Debug
        Info
        Warning
        Error
        Fatal
    }
    class LogOutput {
        <<enumeration>>
        Console
        File
    }

    ThreadSafeLogger ..|> ILogger
    AppLogger --|> ThreadSafeLogger
    LogEntryStream o-- ILogger
    ThreadSafeLogger *-- LoggerSettings
    ILogger ..> LoggerSettings
    LoggerSettings ..> LogLevel
    LoggerSettings ..> LogOutput
```

### Модуль data_model

Исходник: [`docs/pics/class_data_model.mmd`](docs/pics/class_data_model.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class TimePoint {
        <<struct>>
        +string time_
        +double value_
        +kSchema$
    }
    class TimelineData {
        +string name_
        +vector~TimePoint~ points_
        +kSchema$
    }

    TimelineData *-- TimePoint
```

### Модуль parser

Исходник: [`docs/pics/class_parser.mmd`](docs/pics/class_parser.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class IParser {
        <<interface>>
        +~IParser() virtual
        +Load(source) TimelineData
        +ListSubSources(path) vector~string~
    }
    class IParserRegistry {
        <<interface>>
        +~IParserRegistry() virtual
        +Get(extension) shared_ptr~IParser~
        +SupportedExtensions() vector~string~
    }
    class ParserBase {
        <<abstract>>
        +ParserBase(logger)
        #shared_ptr~ILogger~ logger_
    }
    class JsonParser {
        +JsonParser(logger)
        +~JsonParser()
        +Load(source) TimelineData
    }
    class DatabaseParser {
        +DatabaseParser(logger, manager)
        +~DatabaseParser()
        +Load(source) TimelineData
        +ListSubSources(path) vector~string~
        -shared_ptr~IDatabaseManager~ manager_
        -atomic~int~ nextConnId_$
    }
    class ParserRegistry {
        +ParserRegistry(logger)
        +Register(extension, parser) void
        +Get(extension) shared_ptr~IParser~
        +SupportedExtensions() vector~string~
        -map~string, IParser~ parsers_
        -shared_ptr~ILogger~ logger_
    }

    class ILogger {
        <<logger>>
    }
    class IDatabaseManager {
        <<database_module>>
    }
    class TimelineData {
        <<data_model>>
    }

    ParserBase ..|> IParser
    JsonParser --|> ParserBase
    DatabaseParser --|> ParserBase
    ParserRegistry ..|> IParserRegistry
    ParserRegistry o-- IParser
    ParserBase o-- ILogger
    ParserRegistry o-- ILogger
    DatabaseParser o-- IDatabaseManager
    IParser ..> TimelineData
```

### Модуль database_module

Исходник: [`docs/pics/class_database.mmd`](docs/pics/class_database.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class IDatabase {
        <<interface>>
        +~IDatabase() virtual
        +Open(path) bool
        +Close() void
        +Tables() vector~string~
        +Query(sql, rowFn) void
    }
    class IDatabaseManager {
        <<interface>>
        +~IDatabaseManager() virtual
        +Create(connectionName) shared_ptr~IDatabase~
    }
    class SqliteDB {
        +SqliteDB(connectionName, logger)
        +~SqliteDB()
        +Open(path) bool
        +Close() void
        +Tables() vector~string~
        +Query(sql, rowFn) void
        -QSqlDatabase db_
        -shared_ptr~ILogger~ logger_
    }
    class SqliteDBManager {
        +SqliteDBManager(logger)
        +~SqliteDBManager()
        +Create(connectionName) shared_ptr~IDatabase~
        -shared_ptr~ILogger~ logger_
    }

    class ILogger {
        <<logger>>
    }

    SqliteDB ..|> IDatabase
    SqliteDBManager ..|> IDatabaseManager
    SqliteDBManager ..> IDatabase : creates
    SqliteDB o-- ILogger
    SqliteDBManager o-- ILogger
```

### Модуль chart

Исходник: [`docs/pics/class_chart.mmd`](docs/pics/class_chart.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class BuilderOptions {
        <<struct>>
        +bool aggregate
        +const IPalette* palette
    }
    class IChartBuilder {
        <<interface>>
        +~IChartBuilder() virtual
        +Configure(options) void
        +Build(data) unique_ptr~QChart~
    }
    class BarChartBuilder {
        +BarChartBuilder(logger)
        +Configure(options) void
        +Build(data) unique_ptr~QChart~
        -const IPalette* palette_
        -shared_ptr~ILogger~ logger_
    }
    class PieChartBuilder {
        +PieChartBuilder(logger)
        +Configure(options) void
        +Build(data) unique_ptr~QChart~
        -bool aggregate_
        -const IPalette* palette_
        -shared_ptr~ILogger~ logger_
    }
    class LineChartBuilder {
        +LineChartBuilder(logger)
        +Configure(options) void
        +Build(data) unique_ptr~QChart~
        -const IPalette* palette_
        -shared_ptr~ILogger~ logger_
    }
    class AreaChartBuilder {
        +AreaChartBuilder(logger)
        +Configure(options) void
        +Build(data) unique_ptr~QChart~
        -const IPalette* palette_
        -shared_ptr~ILogger~ logger_
    }
    class GanttChartBuilder {
        +GanttChartBuilder(logger)
        +Configure(options) void
        +Build(data) unique_ptr~QChart~
        -const IPalette* palette_
        -shared_ptr~ILogger~ logger_
    }

    class IPalette {
        <<style>>
    }
    class ILogger {
        <<logger>>
    }
    class TimelineData {
        <<data_model>>
    }

    BarChartBuilder ..|> IChartBuilder
    PieChartBuilder ..|> IChartBuilder
    LineChartBuilder ..|> IChartBuilder
    AreaChartBuilder ..|> IChartBuilder
    GanttChartBuilder ..|> IChartBuilder
    IChartBuilder ..> BuilderOptions
    IChartBuilder ..> TimelineData
    BuilderOptions o-- IPalette
    BarChartBuilder o-- IPalette
    PieChartBuilder o-- IPalette
    LineChartBuilder o-- IPalette
    AreaChartBuilder o-- IPalette
    GanttChartBuilder o-- IPalette
    BarChartBuilder o-- ILogger
    PieChartBuilder o-- ILogger
    LineChartBuilder o-- ILogger
    AreaChartBuilder o-- ILogger
    GanttChartBuilder o-- ILogger
```

### Модуль style

Исходник: [`docs/pics/class_style.mmd`](docs/pics/class_style.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class IPalette {
        <<interface>>
        +~IPalette() virtual
        +ColorFor(index, total) QColor
    }
    class IChartStyle {
        <<interface>>
        +Apply(chart) void
    }
    class ColorStyle {
        +ColorStyle(palette, logger)
        +ColorFor(index, total) QColor
        +Apply(chart) void
        -QList~QColor~ palette_
        -shared_ptr~ILogger~ logger_
    }
    class GrayscaleStyle {
        +GrayscaleStyle(logger)
        +ColorFor(index, total) QColor
        +Apply(chart) void
        -shared_ptr~ILogger~ logger_
    }

    class ILogger {
        <<logger>>
    }

    IChartStyle --|> IPalette
    ColorStyle ..|> IChartStyle
    GrayscaleStyle ..|> IChartStyle
    ColorStyle o-- ILogger
    GrayscaleStyle o-- ILogger
```

### Модуль gui

Исходник: [`docs/pics/class_gui.mmd`](docs/pics/class_gui.mmd)

```mermaid
---
config:
  look: classic
---
classDiagram
    direction TB

    class IChartExporter {
        <<interface>>
        +~IChartExporter() virtual
        +Export(scene, path) bool
        +FileFilter() QString
    }
    class PdfExporter {
        +Export(scene, path) bool
        +FileFilter() QString
    }
    class MainWindow {
        +MainWindow(builders, styles, exporters, registry, logger, parent)
        +~MainWindow()
        +onFileSelected(index) void
        +onSaveChart() void
        +onChooseFolder() void
        +refresh() void
        +onError(message) void
        +toggleTheme() void
        +applyTheme() void
        -loadFile(path) void
        -setRoot(path) void
        -setChart(chart) void
        -buildChart(data) unique_ptr~QChart~
        -applyChartTheme(chart) void
        -updatePaletteButton(color) void
        -buildPalettePopover() void
        -installRowWidgets(parent) void
        -updateRowSelection() void
        -BuilderFactory builders_
        -StyleFactory styles_
        -ExporterFactory exporters_
        -shared_ptr~IParserRegistry~ registry_
        -shared_ptr~ILogger~ logger_
        -unique_ptr~ChartModel~ model_
        -QTreeView* treeView_
        -QChartView* chartView_
        -QButtonGroup* chartTypeGroup_
        -QToolButton* paletteButton_
        -QMenu* paletteMenu_
        -QToolButton* themeButton_
        -QLabel* plotTitle_
        -QLabel* statusInfo_
        -QLabel* pathLabel_
        -FileItemDelegate* fileDelegate_
        -bool darkTheme_
        -QColor accent_
    }
    class ChartModel {
        +ChartModel(registry, logger, parent)
        +data() TimelineData&
        +hasData() bool
        +builder() string&
        +style() string&
        +aggregate() bool
        +listSubSources(path) vector~string~
        +setSource(source) void
        +setBuilder(builder) void
        +setStyle(style) void
        +setAggregate(aggregate) void
        +dataChanged() signal
        +renderOptionsChanged() signal
        +errorOccurred(message) signal
        -SourcePath(source) string$
        -shared_ptr~IParserRegistry~ registry_
        -shared_ptr~ILogger~ logger_
        -optional~DataSlot~ dataSlot_
        -optional~TablesSlot~ tablesSlot_
        -string builder_
        -string style_
        -bool aggregate_
    }
    class FileItemDelegate {
        +FileItemDelegate(parent)
        +sizeHint(option, index) QSize
        +paint(painter, option, index) void
    }
    class FileRowWidget {
        +FileRowWidget(kind, isDir, name, info, index, parent)
        +setSelected(selected) void
        +applyTheme(dark, accent) void
        +index() QModelIndex
        -updateColors() void
        -FileKind kind_
        -bool isDir_
        -bool selected_
        -bool dark_
        -QColor accent_
        -QPersistentModelIndex index_
        -GlyphIcon* icon_
        -QLabel* nameLabel_
        -QLabel* sizeLabel_
        -QLabel* dateLabel_
    }
    class GlyphIcon {
        +GlyphIcon(kind, parent)
        +setColor(color) void
        #paintEvent(event) void
        -FileKind kind_
        -QColor color_
    }
    class ScrollableChartView {
        +ScrollableChartView(parent)
        +resetView() void
        #resizeEvent(event) void
        #wheelEvent(event) void
        -layoutChart() void
        -qreal contentScale_
    }
    class TableSelectDialog {
        +TableSelectDialog(tables, parent)
        +selectedTable() QString
        -QListWidget* list_
    }
    class ToggleSwitch {
        +ToggleSwitch(parent)
        +sizeHint() QSize
        +pos() qreal
        +setPos(p) void
        +setAccent(color) void
        #paintEvent(event) void
        -qreal pos_
        -QColor accent_
    }
    class FileKind {
        <<enumeration>>
        Folder
        Json
        Sqlite
        Generic
    }

    class IChartBuilder {
        <<chart>>
    }
    class IChartStyle {
        <<style>>
    }
    class IParserRegistry {
        <<parser>>
    }
    class ILogger {
        <<logger>>
    }
    class TimelineData {
        <<data_model>>
    }

    PdfExporter ..|> IChartExporter
    MainWindow *-- ChartModel
    MainWindow *-- FileItemDelegate
    MainWindow ..> FileRowWidget : creates
    MainWindow ..> ScrollableChartView : creates
    MainWindow ..> TableSelectDialog : creates
    MainWindow o-- IChartExporter
    MainWindow o-- IChartBuilder
    MainWindow o-- IChartStyle
    MainWindow o-- IParserRegistry
    MainWindow o-- ILogger
    FileItemDelegate ..> FileRowWidget
    FileRowWidget *-- GlyphIcon
    FileRowWidget ..> FileKind
    GlyphIcon ..> FileKind
    ChartModel o-- IParserRegistry
    ChartModel o-- ILogger
    ChartModel *-- TimelineData
```

## Диаграмма сигналов и слотов

Взаимодействие виджетов, модели и представления через механизм сигналов/слотов Qt
(паттерн Model/View). Исходник: [`docs/pics/signal_slot.mmd`](docs/pics/signal_slot.mmd).

- Сплошная стрелка с меткой — `connect(сигнал → слот)`.
- Пунктир `emit` — слот модели эмитит сигнал.
- Пунктир `loadFile()` — прямой вызов (не через `connect`).

```mermaid
---
config:
  look: classic
---
flowchart LR
    classDef sig fill:#f6d9ec,stroke:#b5497f,color:#000
    classDef slot fill:#d9e8f6,stroke:#2f78b5,color:#000

    subgraph W["Виджеты Qt (источники сигналов)"]
        tree["QTreeView treeView_"]
        folder["QPushButton folderBtn"]
        save["QPushButton saveBtn"]
        theme["QToolButton themeButton_"]
        types["QButtonGroup chartTypeGroup_"]
        swatch["QToolButton swatch (палитра)"]
        fsm["QFileSystemModel"]
        sel["QItemSelectionModel"]
        toggle["ToggleSwitch"]
        dlgBox["QDialogButtonBox"]
    end

    subgraph M["ChartModel — модель / контроллер"]
        setSource["setSource()"]:::slot
        setBuilder["setBuilder()"]:::slot
        setStyle["setStyle()"]:::slot
        setAgg["setAggregate()"]:::slot
        sigData(["dataChanged()"]):::sig
        sigOpts(["renderOptionsChanged()"]):::sig
        sigErr(["errorOccurred(msg)"]):::sig
    end

    subgraph V["MainWindow — представление"]
        onFile["onFileSelected()"]:::slot
        onFolder["onChooseFolder()"]:::slot
        onSave["onSaveChart()"]:::slot
        onTheme["toggleTheme()"]:::slot
        refresh["refresh()"]:::slot
        onErr["onError()"]:::slot
        instRows["installRowWidgets()"]
        updSel["updateRowSelection()"]
    end

    subgraph D["TableSelectDialog"]
        accept["accept()"]:::slot
        reject["reject()"]:::slot
    end

    tree -- "clicked(index)" --> onFile
    folder -- "clicked()" --> onFolder
    save -- "clicked()" --> onSave
    theme -- "clicked()" --> onTheme
    tree -- "expanded(index)" --> instRows
    fsm -- "directoryLoaded(path)" --> instRows
    sel -- "selectionChanged()" --> updSel

    types -- "buttonClicked(btn)" --> setBuilder
    swatch -- "clicked()" --> setStyle

    onFile -. "loadFile()" .-> setSource

    setSource -. emit .-> sigData
    setSource -. emit .-> sigErr
    setBuilder -. emit .-> sigOpts
    setStyle -. emit .-> sigOpts
    setAgg -. emit .-> sigOpts

    sigData -- "dataChanged()" --> refresh
    sigOpts -- "renderOptionsChanged()" --> refresh
    sigErr -- "errorOccurred(msg)" --> onErr

    dlgBox -- "accepted()" --> accept
    dlgBox -- "rejected()" --> reject

    toggle -- "toggled(on)" --> toggle
```

### Таблица соединений

| Источник (сигнал)                              | Приёмник (слот)               | Где                      |
|------------------------------------------------|-------------------------------|--------------------------|
| `treeView_::clicked(index)`                    | `MainWindow::onFileSelected`  | `mainwindow.cpp`         |
| `folderBtn::clicked()`                         | `MainWindow::onChooseFolder`  | `mainwindow.cpp`         |
| `saveBtn::clicked()`                           | `MainWindow::onSaveChart`     | `mainwindow.cpp`         |
| `themeButton_::clicked()`                      | `MainWindow::toggleTheme`     | `mainwindow.cpp`         |
| `treeView_::expanded(index)`                   | λ → `installRowWidgets`       | `mainwindow.cpp`         |
| `QFileSystemModel::directoryLoaded(path)`      | λ → `installRowWidgets`       | `mainwindow.cpp` (setRoot) |
| `QItemSelectionModel::selectionChanged()`      | λ → `updateRowSelection`      | `mainwindow.cpp` (setRoot) |
| `chartTypeGroup_::buttonClicked(btn)`          | λ → `ChartModel::setBuilder`  | `mainwindow.cpp`         |
| `swatch::clicked()`                            | λ → `ChartModel::setStyle`    | `mainwindow.cpp` (палитра) |
| `ChartModel::dataChanged()`                    | `MainWindow::refresh`         | `mainwindow.cpp`         |
| `ChartModel::renderOptionsChanged()`           | `MainWindow::refresh`         | `mainwindow.cpp`         |
| `ChartModel::errorOccurred(msg)`               | `MainWindow::onError`         | `mainwindow.cpp`         |
| `QDialogButtonBox::accepted()`                 | `QDialog::accept`             | `table_select_dialog.cpp`|
| `QDialogButtonBox::rejected()`                 | `QDialog::reject`             | `table_select_dialog.cpp`|
| `ToggleSwitch::toggled(on)`                    | λ (анимация ползунка)         | `toggle_switch.cpp`      |

Сигналы `ChartModel` эмитятся из слотов-мутаторов: `setSource` → `dataChanged` / `errorOccurred`;
`setBuilder` / `setStyle` / `setAggregate` → `renderOptionsChanged`.

> Примечание: слот `ChartModel::setAggregate` определён, но в текущей сборке к нему не
> подключён ни один виджет; `setSource` вызывается напрямую из `loadFile()`, а не через `connect`.

## Тестирование

### Пользовательские тест-кейсы

Тестовые данные лежат в [`test_data`](test_data) (JSON-ряды) и
[`test_data/InputData`](test_data/InputData) (БД SQLite)

#### Case №1
Проверка запуска приложения и первичного выбора папки
* Входные параметры: каталог `test_data` с файлами `.json` и `.sqlite`
    * Шаг 1 — выполнить запуск `./programming_tech_lab_3`
    * Шаг 2 — в открывшемся диалоге первичного выбора папки указать каталог `test_data`
* Результат: сначала появляется диалог выбора папки; после подтверждения открывается главное окно с двумя областями — деревом файлов слева и пустой областью графика справа; приложение не завершается

#### Case №2
Проверка отображения всех файлов и иконок
* Входные параметры: каталог с файлами разных типов (`.json`, `.sqlite` и, например, `.txt`)
    * Шаг 1 — нажать кнопку выбора папки и указать этот каталог
* Результат: в дереве видны **все** файлы и папки; json/sqlite показаны со своими иконками формата, остальные файлы — с обобщённой иконкой

#### Case №2.1
Проверка реакции на выбор неподдерживаемого файла
* Входные параметры: в дереве присутствует файл неподдерживаемого формата (например, `.txt`)
    * Шаг 1 — выбрать такой файл в дереве
* Результат: график не строится и сообщение об ошибке не появляется; ранее построенный график сохраняется (рисуются только json и sqlite)

#### Case №3
Проверка построения графика из JSON
* Входные параметры: файл `test_data/MONTHLY_TEMPERATURE.json`
    * Шаг 1 — выбрать в дереве `MONTHLY_TEMPERATURE.json`
* Результат: строится график; над областью отображается имя ряда, в статус-баре — число точек

#### Case №4
Проверка выбора БД с несколькими таблицами
* Входные параметры: файл `test_data/InputData/WEATHER_STATION.sqlite` (содержит 3 таблицы)
    * Шаг 1 — выбрать в дереве `WEATHER_STATION.sqlite`
    * Шаг 2 — в появившемся диалоге выбрать таблицу из списка
* Результат: открывается диалог выбора таблицы со списком из 3 таблиц; после выбора строится график по выбранной таблице

#### Case №5
Проверка выбора БД с одной таблицей
* Входные параметры: файл `test_data/InputData/BLOOD_SUGAR.sqlite` (одна таблица)
    * Шаг 1 — выбрать в дереве `BLOOD_SUGAR.sqlite`
* Результат: диалог выбора таблицы не появляется, график строится сразу по единственной таблице

#### Case №6
Проверка смены типа графика
* Входные параметры: график уже построен (например, из Case №3)
    * Шаг 1 — переключить тип графика в сегмент-контроле (Bar / Pie / Line / Area / Gantt)
* Результат: график перестраивается под выбранный тип из тех же данных, файл повторно не загружается

#### Case №7
Проверка смены палитры / стиля
* Входные параметры: график уже построен
    * Шаг 1 — открыть поповер палитр и выбрать другую палитру
    * Шаг 2 — выбрать стиль «Оттенки серого»
* Результат: график перекрашивается согласно выбранной палитре; в режиме «Оттенки серого» — градации серого

#### Case №8
Проверка переключения темы
* Входные параметры: график уже построен
    * Шаг 1 — нажать кнопку переключения темы
* Результат: интерфейс и область графика переключаются между светлой и тёмной темой

#### Case №9
Проверка экспорта графика в PDF
* Входные параметры: график построен
    * Шаг 1 — выполнить сохранение графика
    * Шаг 2 — в диалоге выбрать место сохранения и подтвердить
* Результат: по указанному пути создаётся PDF-файл с текущим графиком

#### Case №10
Проверка обработки JSON с синтаксической ошибкой
* Входные параметры: файл `test_data/invalid/broken_syntax.json` (нарушен синтаксис JSON)
    * Шаг 1 — выбрать файл в дереве
* Результат: приложение не завершается; выводится сообщение об ошибке загрузки, ранее построенный график сохраняется

#### Case №11
Проверка обработки JSON с неверной схемой
* Входные параметры: файл `test_data/invalid/wrong_schema.json` (валидный JSON, но без полей `name`/`points`)
    * Шаг 1 — выбрать файл в дереве
* Результат: выводится сообщение об ошибке загрузки; приложение продолжает работу

#### Case №12
Проверка обработки JSON с нечисловым значением
* Входные параметры: файл `test_data/invalid/bad_value.json` (поле `Value` содержит строку вместо числа)
    * Шаг 1 — выбрать файл в дереве
* Результат: выводится сообщение об ошибке загрузки; приложение продолжает работу

#### Case №13
Проверка обработки пустого файла
* Входные параметры: файл `test_data/invalid/empty.json` (нулевой размер)
    * Шаг 1 — выбрать файл в дереве
* Результат: выводится сообщение об ошибке загрузки; приложение не завершается

#### Case №14
Проверка обработки файла, не являющегося базой SQLite
* Входные параметры: файл `test_data/invalid/not_a_database.sqlite` (текст с расширением `.sqlite`)
    * Шаг 1 — выбрать файл в дереве
* Результат: выводится сообщение об ошибке открытия базы; приложение продолжает работу