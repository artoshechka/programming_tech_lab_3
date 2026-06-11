/// @file mainwindow.hpp
/// @brief Главное окно приложения
/// @author Artemenko Anton
#ifndef GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
#define GUID_c9d0e1f2_a3b4_5678_cdef_789012345678

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QToolButton>
#include <QTreeView>
#include <QtCharts/QChartView>
#include <chart/ichart_builder.hpp>
#include <functional>
#include <logger/ilogger.hpp>
#include <map>
#include <memory>
#include <parser/iparser.hpp>
#include <parser/iparser_registry.hpp>
#include <string>
#include <style/ichart_style.hpp>

QT_CHARTS_USE_NAMESPACE

namespace gui
{

/// @brief Фабрика построителей графиков: имя -> функция, создающая IChartBuilder.
using BuilderFactory = std::map<std::string, std::function<std::shared_ptr<chart::IChartBuilder>()>>;
/// @brief Фабрика стилей графиков: имя -> функция, создающая IChartStyle.
using StyleFactory = std::map<std::string, std::function<std::shared_ptr<style::IChartStyle>()>>;
/// @brief Фабрика парсеров: расширение -> функция, создающая IParser. Используется только в композиции.
using ParserFactory = std::map<std::string, std::function<std::shared_ptr<parser::IParser>()>>;

class ChartModel;

/// @brief Главное окно — представление (View) в Qt Model/View.
/// @details Сигналы виджетов связаны со слотами-мутаторами ChartModel (роль контроллера),
///          а сигналы модели — со слотом refresh(), который перестраивает график. Представление
///          само не загружает и не кэширует данные — только отображает состояние модели.
class MainWindow : public QMainWindow
{
    Q_OBJECT
   public:
    /// @brief Конструктор главного окна.
    /// @param[in] builders Фабрика построителей графиков (имя -> создатель).
    /// @param[in] styles Фабрика стилей графиков (имя -> создатель).
    /// @param[in] registry Реестр парсеров для загрузки данных по расширению.
    /// @param[in] logger Логгер для диагностики действий пользователя; допускается nullptr.
    /// @param[in] parent Родительский виджет (по умолчанию nullptr).
    explicit MainWindow(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                        const std::shared_ptr<logger::ILogger>& logger = nullptr, QWidget* parent = nullptr);
    /// @brief Виртуальный деструктор.
    ~MainWindow() override;

   private slots:
    /// @brief Обрабатывает клик по элементу дерева: пропускает папки, загружает файл.
    /// @param[in] index Индекс выбранного элемента модели файловой системы.
    void onFileSelected(const QModelIndex& index);
    /// @brief Открывает диалог сохранения, рендерит текущий график в PDF через QPdfWriter.
    void onSavePdf();
    /// @brief Открывает диалог выбора папки; обновляет дерево.
    void onChooseFolder();
    /// @brief Перестраивает график из текущего состояния модели (реакция на сигналы модели).
    void refresh();
    /// @brief Показывает пользователю ошибку, пришедшую сигналом модели.
    /// @param[in] message Текст ошибки.
    void onError(const QString& message);
    /// @brief Переключает светлую/тёмную тему оформления и обновляет подпись кнопки.
    void toggleTheme();

   private:
    /// @brief Запрашивает под-источники, при необходимости показывает диалог выбора, затем задаёт источник модели.
    /// @param[in] path Путь к файлу данных.
    void loadFile(const QString& path);
    /// @brief Пересоздаёт QFileSystemModel с фильтрами по реестру парсеров и задаёт корень дерева.
    /// @param[in] path Путь к папке.
    void setRoot(const QString& path);
    /// @brief Передаёт новый график в QChartView, удаляя предыдущий.
    /// @param[in] chart Новый график; владение передаётся QChartView через release().
    void setChart(std::unique_ptr<QChart> chart);
    /// @brief Строит QChart из данных по текущим builder/style/aggregate модели.
    /// @param[in] data Данные временного ряда.
    /// @return Владеющий указатель на построенный график.
    std::unique_ptr<QChart> buildChart(const data::TimelineData& data);

    BuilderFactory builders_;                            ///< Фабрика построителей графиков (рендер во View).
    StyleFactory styles_;                                ///< Фабрика стилей графиков (рендер во View).
    std::shared_ptr<parser::IParserRegistry> registry_;  ///< Реестр парсеров (для запроса поддерживаемых расширений).
    std::shared_ptr<logger::ILogger> logger_;            ///< Логгер для диагностики (может быть nullptr).
    std::unique_ptr<ChartModel> model_;                  ///< Наблюдаемая модель состояния и данных графика.

    QTreeView* treeView_ = nullptr;        ///< Дерево файлов с данными.
    QChartView* chartView_ = nullptr;      ///< Область отображения графика.
    QComboBox* chartCombo_ = nullptr;      ///< Выбор типа построителя графика.
    QComboBox* styleCombo_ = nullptr;      ///< Выбор стиля графика.
    QCheckBox* aggregateCheck_ = nullptr;  ///< Чекбокс включения агрегации (актуален для Pie).
    QToolButton* themeButton_ = nullptr;   ///< Кнопка переключения светлой/тёмной темы.
    QLabel* plotTitle_ = nullptr;          ///< Заголовок над областью графика (имя ряда).
    QLabel* statusInfo_ = nullptr;         ///< Правый индикатор статус-бара (число точек ряда).
    bool darkTheme_ = false;               ///< Активна ли тёмная тема.
};

}  // namespace gui

#endif  // GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
