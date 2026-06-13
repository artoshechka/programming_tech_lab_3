# Лабораторная работа по предмету: "Разработка средств защиты информации"
## Тема: "QtWidgets - Печать графиков"
> 4 курс 2 семестр \
> Студент группы 932223 - **Артеменко Антон Дмитриевич**

## Документация (UML)

Исходники диаграмм — в [`docs/pics`](docs/pics). Ниже они отображаются через Mermaid.

### Диаграмма пакетов

Структура модулей, зависимости и границы ответственности.
Исходник: [`docs/pics/package_diagram.mmd`](docs/pics/package_diagram.mmd)

```mermaid
graph TD
    main["main<br/>(точка входа)"]
    gui["gui<br/>View + ChartModel + композиция"]
    ioc["ioc_container<br/>DI-контейнер"]
    chart["chart<br/>построители графиков + агрегация"]
    style["style<br/>палитры и темы графика"]
    parser["parser<br/>загрузка данных по расширению"]
    database["database_module<br/>доступ к SQLite"]
    data_model["data_model<br/>доменная модель TimelineData"]
    logger["logger<br/>логирование (без Qt)"]

    main --> gui
    gui --> chart
    gui --> style
    gui --> parser
    gui --> database
    gui --> data_model
    gui --> logger
    gui --> ioc
    chart --> data_model
    chart --> style
    chart --> logger
    style --> logger
    parser --> data_model
    parser --> database
    parser --> logger
    database --> logger
```

### Диаграмма классов

Ключевые сущности и связи (реализация интерфейсов, наследование, композиция, агрегация).
Исходник: [`docs/pics/class_diagram.mmd`](docs/pics/class_diagram.mmd)

```mermaid
classDiagram
    direction LR
    class ILogger { <<interface>> +Log() +SetSettings() +GetSettings() }
    class ThreadSafeLogger
    class LogEntryStream
    ThreadSafeLogger ..|> ILogger
    LogEntryStream o-- ILogger

    class TimelineData { +string name_ +vector~TimePoint~ points_ }
    class TimePoint { +string time_ +double value_ }
    TimelineData *-- TimePoint

    class IParser { <<interface>> +Load() +ListSubSources() }
    class ParserBase
    class JsonParser
    class DatabaseParser
    class IParserRegistry { <<interface>> +Get() +SupportedExtensions() }
    class ParserRegistry
    ParserBase ..|> IParser
    JsonParser --|> ParserBase
    DatabaseParser --|> ParserBase
    ParserRegistry ..|> IParserRegistry
    ParserRegistry o-- IParser
    IParser --> TimelineData
    DatabaseParser o-- IDatabaseManager

    class IDatabase { <<interface>> +Open() +Tables() +Query() }
    class IDatabaseManager { <<interface>> +Create() }
    class SqliteDB
    class SqliteDBManager
    SqliteDB ..|> IDatabase
    SqliteDBManager ..|> IDatabaseManager
    SqliteDBManager --> IDatabase : creates

    class IChartBuilder { <<interface>> +Configure() +Build() }
    class BarChartBuilder
    class PieChartBuilder
    BarChartBuilder ..|> IChartBuilder
    PieChartBuilder ..|> IChartBuilder
    IChartBuilder --> TimelineData
    IChartBuilder --> IPalette

    class IPalette { <<interface>> +ColorFor() }
    class IChartStyle { <<interface>> +Apply() }
    class ColorStyle
    class GrayscaleStyle
    IChartStyle --|> IPalette
    ColorStyle ..|> IChartStyle
    GrayscaleStyle ..|> IChartStyle

    class MainWindow
    class ChartModel
    MainWindow *-- ChartModel
    MainWindow o-- IChartBuilder
    MainWindow o-- IChartStyle
    MainWindow o-- IParserRegistry
    ChartModel o-- IParserRegistry
    ChartModel *-- TimelineData
    ChartModel o-- ILogger
    MainWindow o-- ILogger
```

### Диаграммы последовательностей

**Запуск и композиция (IoC).** Исходник: [`docs/pics/sequence_startup.mmd`](docs/pics/sequence_startup.mmd)

```mermaid
sequenceDiagram
    actor User
    participant main
    participant Compositor as app_compositor
    participant IOC as IOCContainer
    participant MW as MainWindow

    User->>main: запуск
    main->>Compositor: CreateMainWindow()
    Compositor->>IOC: RegisterInstance(ILogger)
    Compositor->>IOC: RegisterInstance(IDatabaseManager)
    Compositor->>IOC: RegisterFactory(JsonParser / DatabaseParser)
    Compositor->>IOC: Resolve реестра парсеров
    IOC-->>Compositor: IParserRegistry
    Compositor->>Compositor: собрать BuilderFactory / StyleFactory
    Compositor->>MW: new MainWindow(builders, styles, registry, logger)
    MW-->>Compositor: указатель на окно
    Compositor-->>main: MainWindow*
    main->>MW: show()
```

**Выбор файла → загрузка → построение графика.** Исходник: [`docs/pics/sequence_load_render.mmd`](docs/pics/sequence_load_render.mmd)

```mermaid
sequenceDiagram
    actor User
    participant MW as MainWindow (View)
    participant Model as ChartModel
    participant Reg as IParserRegistry
    participant P as IParser
    participant B as IChartBuilder
    participant S as IChartStyle

    User->>MW: клик по файлу в дереве
    MW->>Model: listSubSources(path)
    Model-->>MW: список под-источников
    opt несколько таблиц
        MW->>User: TableSelectDialog
        User-->>MW: выбранная таблица
    end
    MW->>Model: setSource(source)
    Model->>Reg: Get(extension)
    Reg-->>Model: IParser
    Model->>P: Load(source)
    P-->>Model: TimelineData
    Model->>Model: кэш по mtime
    Model-->>MW: signal dataChanged()
    MW->>Model: data()
    Model-->>MW: const TimelineData&
    MW->>B: Configure(options) + Build(data)
    B-->>MW: unique_ptr~QChart~
    MW->>S: Apply(chart)
    MW->>MW: setChart(chart) + обновить заголовок/статус
```

**Смена опций отрисовки.** Исходник: [`docs/pics/sequence_render_options.mmd`](docs/pics/sequence_render_options.mmd)

```mermaid
sequenceDiagram
    actor User
    participant MW as MainWindow (View)
    participant Model as ChartModel
    participant B as IChartBuilder
    participant S as IChartStyle

    User->>MW: выбор типа / стиля / переключение агрегации
    MW->>Model: setBuilder() / setStyle() / setAggregate()
    alt значение изменилось
        Model-->>MW: signal renderOptionsChanged()
        MW->>Model: data()
        Model-->>MW: const TimelineData&
        MW->>B: Configure(options) + Build(data)
        B-->>MW: unique_ptr~QChart~
        MW->>S: Apply(chart)
        MW->>MW: setChart(chart)
    else значение не изменилось
        Model-->>MW: (без сигнала)
    end
```
