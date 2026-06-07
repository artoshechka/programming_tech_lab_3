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
#include <database_module/idatabase_manager.hpp>
#include <functional>
#include <map>
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
    /// @param[in] dbManager Менеджер БД для работы с источниками SQLite.
    /// @param[in] parent Родительский виджет (по умолчанию nullptr).
    explicit MainWindow(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                        std::shared_ptr<database::manager::IDatabaseManager> dbManager, QWidget* parent = nullptr);
    /// @brief Виртуальный деструктор.
    ~MainWindow() override;

   private slots:
    /// @brief Слот выбора файла в дереве.
    /// @param[in] index Индекс выбранного элемента модели файловой системы.
    void onFileSelected(const QModelIndex& index);
    /// @brief Слот сохранения текущего графика в PDF.
    void onSavePdf();
    /// @brief Слот перерисовки графика при смене построителя или стиля.
    void onRedraw();
    /// @brief Слот выбора рабочей папки с данными.
    void onChooseFolder();

   private:
    /// @brief Загружает источник данных и строит график.
    /// @param[in] path Путь к файлу данных.
    void loadFile(const QString& path);
    /// @brief Устанавливает корневую папку дерева файлов.
    /// @param[in] path Путь к папке.
    void setRoot(const QString& path);
    /// @brief Заменяет текущий график в области отображения.
    /// @param[in] chart Новый график (владение передаётся представлению).
    void setChart(QChart* chart);

    std::shared_ptr<parser::IParserRegistry> registry_;  ///< Реестр парсеров (для запроса поддерживаемых расширений).
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
