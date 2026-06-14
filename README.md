# Лабораторная работа по предмету: "Разработка средств защиты информации"
## Тема: "QtWidgets - Печать графиков"
> 4 курс 2 семестр \
> Студент группы 932223 - **Артеменко Антон Дмитриевич**

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