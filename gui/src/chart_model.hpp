/// @file chart_model.hpp
/// @brief Наблюдаемая модель приложения (Qt Model/View)
/// @author Artemenko Anton
#ifndef GUID_b2c3d4e5_f6a7_8901_bcde_f23456789012
#define GUID_b2c3d4e5_f6a7_8901_bcde_f23456789012

#include <QObject>
#include <QString>
#include <data_model/src/timeline_data.hpp>
#include <filesystem>
#include <logger/ilogger.hpp>
#include <memory>
#include <optional>
#include <parser/iparser_registry.hpp>
#include <string>
#include <vector>

namespace gui
{

/// @brief Наблюдаемая модель: хранит состояние графика и данные, уведомляет об изменениях сигналами.
/// @details Активная модель Qt Model/View. Роль контроллера выполняют сигналы/слоты: ввод от View
///          приходит в слоты-мутаторы, изменения модель транслирует наружу сигналами. Данные
///          кэшируются с инвалидацией по mtime — повторный выбор того же файла не перечитывает IO.
class ChartModel : public QObject
{
    Q_OBJECT
   public:
    /// @brief Конструктор модели.
    /// @param[in] registry Реестр парсеров для загрузки данных по расширению.
    /// @param[in] logger Логгер для диагностики; допускается nullptr.
    /// @param[in] parent Родительский QObject (по умолчанию nullptr).
    explicit ChartModel(std::shared_ptr<parser::IParserRegistry> registry,
                        std::shared_ptr<logger::ILogger> logger = nullptr, QObject* parent = nullptr);

    /// @brief Возвращает закэшированные данные текущего источника.
    /// @return Ссылка на данные; вызывать только при hasData() == true.
    const data::TimelineData& data() const;
    /// @brief Есть ли загруженные данные.
    [[nodiscard]] bool hasData() const { return dataSlot_.has_value(); }
    /// @brief Текущее имя построителя графика.
    [[nodiscard]] const std::string& builder() const { return builder_; }
    /// @brief Текущее имя стиля графика.
    [[nodiscard]] const std::string& style() const { return style_; }
    /// @brief Включена ли агрегация.
    [[nodiscard]] bool aggregate() const { return aggregate_; }

    /// @brief Возвращает под-источники файла (таблицы и т.п.) для выбора пользователем.
    /// @param[in] path Путь к файлу-источнику.
    /// @return Имена под-источников; пустой вектор, если их нет или формат одиночный.
    /// @details Запрос без мутации: View спрашивает до setSource(), чтобы дать выбрать под-источник.
    std::vector<std::string> listSubSources(const std::string& path);

   public slots:
    /// @brief Загружает источник, кэширует данные, эмитит dataChanged() либо errorOccurred().
    /// @param[in] source Источник данных (путь, опционально с "|таблица" для SQLite).
    void setSource(const std::string& source);
    /// @brief Меняет построитель; при фактическом изменении эмитит renderOptionsChanged().
    /// @param[in] builder Имя построителя графика.
    void setBuilder(const std::string& builder);
    /// @brief Меняет стиль; при фактическом изменении эмитит renderOptionsChanged().
    /// @param[in] style Имя стиля графика.
    void setStyle(const std::string& style);
    /// @brief Меняет флаг агрегации; при фактическом изменении эмитит renderOptionsChanged().
    /// @param[in] aggregate Включить агрегацию.
    void setAggregate(bool aggregate);

   signals:
    /// @brief Данные источника перечитаны — представление должно перестроить график.
    void dataChanged();
    /// @brief Сменились builder/style/aggregate — представление должно перерисовать график из тех же данных.
    void renderOptionsChanged();
    /// @brief Ошибка загрузки источника; данные модели остаются прежними.
    /// @param[in] message Текст ошибки для отображения пользователю.
    void errorOccurred(const QString& message);

   private:
    /// @brief Извлекает путь к файлу из source ("путь" либо "путь|таблица").
    static std::string SourcePath(const std::string& source);

    /// @brief Кэш распарсенных данных последнего источника.
    struct DataSlot
    {
        std::string source;                     ///< Источник, для которого закэшированы данные.
        std::filesystem::file_time_type mtime;  ///< Время модификации файла на момент парсинга.
        data::TimelineData data;                ///< Распарсенные данные.
    };

    /// @brief Кэш списка под-источников последнего файла.
    struct TablesSlot
    {
        std::string path;                       ///< Путь к файлу, для которого закэширован список.
        std::filesystem::file_time_type mtime;  ///< Время модификации файла на момент чтения.
        std::vector<std::string> tables;        ///< Имена под-источников.
    };

    std::shared_ptr<parser::IParserRegistry> registry_;  ///< Реестр парсеров по расширению.
    std::shared_ptr<logger::ILogger> logger_;            ///< Логгер для диагностики (может быть nullptr).
    std::optional<DataSlot> dataSlot_;                   ///< Кэш данных последнего источника.
    std::optional<TablesSlot> tablesSlot_;               ///< Кэш под-источников последнего файла.
    std::string builder_;                                ///< Текущее имя построителя графика.
    std::string style_;                                  ///< Текущее имя стиля графика.
    bool aggregate_ = true;                              ///< Текущий флаг агрегации.
};

}  // namespace gui

#endif  // GUID_b2c3d4e5_f6a7_8901_bcde_f23456789012
