/// @file mainwindow.hpp
/// @brief Главное окно приложения
/// @author Artemenko Anton
#ifndef GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
#define GUID_c9d0e1f2_a3b4_5678_cdef_789012345678

#include <QCheckBox>
#include <QComboBox>
#include <QMainWindow>
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

class ChartPresenter;

/// @brief Главное окно приложения.
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
                        std::shared_ptr<logger::ILogger> logger = nullptr, QWidget* parent = nullptr);
    /// @brief Виртуальный деструктор.
    ~MainWindow() override;

   private slots:
    /// @brief Обрабатывает клик по элементу дерева: пропускает папки, загружает файл.
    /// @param[in] index Индекс выбранного элемента модели файловой системы.
    void onFileSelected(const QModelIndex& index);
    /// @brief Открывает диалог сохранения, рендерит текущий график в PDF через QPdfWriter.
    void onSavePdf();
    /// @brief Пересобирает график из кэша при смене построителя, стиля или флага агрегации.
    void onRedraw();
    /// @brief Открывает диалог выбора папки; сбрасывает текущий источник и обновляет дерево.
    void onChooseFolder();

   private:
    /// @brief Запрашивает под-источники, при необходимости показывает диалог выбора, затем строит график.
    /// @param[in] path Путь к файлу данных.
    void loadFile(const QString& path);
    /// @brief Пересоздаёт QFileSystemModel с фильтрами по реестру парсеров и задаёт корень дерева.
    /// @param[in] path Путь к папке.
    void setRoot(const QString& path);
    /// @brief Передаёт новый график в QChartView, удаляя предыдущий.
    /// @param[in] chart Новый график; владение передаётся QChartView через release().
    void setChart(std::unique_ptr<QChart> chart);

    std::shared_ptr<parser::IParserRegistry> registry_;  ///< Реестр парсеров (для запроса поддерживаемых расширений).
    std::shared_ptr<logger::ILogger> logger_;    ///< Логгер для диагностики (может быть nullptr).
    std::unique_ptr<ChartPresenter> presenter_;  ///< Презентер загрузки данных и построения графика.

    QTreeView* treeView_ = nullptr;        ///< Дерево файлов с данными.
    QChartView* chartView_ = nullptr;      ///< Область отображения графика.
    QComboBox* chartCombo_ = nullptr;      ///< Выбор типа построителя графика.
    QComboBox* styleCombo_ = nullptr;      ///< Выбор стиля графика.
    QCheckBox* aggregateCheck_ = nullptr;  ///< Чекбокс включения агрегации (актуален для Pie).
    QString currentSource_;                ///< Текущий источник данных (путь, опционально с "|таблица").
};

}  // namespace gui

#endif  // GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
