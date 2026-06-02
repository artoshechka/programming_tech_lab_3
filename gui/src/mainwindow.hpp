/// @file mainwindow.hpp
/// @brief Главное окно приложения
/// @author Artemenko Anton
#ifndef GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
#define GUID_c9d0e1f2_a3b4_5678_cdef_789012345678

#include <QMainWindow>
#include <QTreeView>
#include <QComboBox>
#include <QtCharts/QChartView>
#include <chart/ichart_builder.hpp>
#include <style/ichart_style.hpp>
#include <parser/iparser_registry.hpp>
#include <database_module/idatabase_manager.hpp>
#include <map>
#include <functional>
#include <string>

QT_CHARTS_USE_NAMESPACE

namespace gui {

using BuilderFactory = std::map<std::string, std::function<std::shared_ptr<chart::IChartBuilder>()>>;
using StyleFactory   = std::map<std::string, std::function<std::shared_ptr<style::IChartStyle>()>>;

class ChartPresenter;

/// @brief Главное окно приложения.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(
        BuilderFactory                                        builders,
        StyleFactory                                          styles,
        std::shared_ptr<parser::IParserRegistry>              registry,
        std::shared_ptr<database::manager::IDatabaseManager>  dbManager,
        QWidget* parent = nullptr
    );
    ~MainWindow() override;

private slots:
    void onFileSelected(const QModelIndex& index);
    void onSavePdf();
    void onRedraw();
    void onChooseFolder();

private:
    void loadFile(const QString& path);
    void setRoot(const QString& path);
    void setChart(QChart* chart);

    std::unique_ptr<ChartPresenter>                       presenter_;
    std::shared_ptr<database::manager::IDatabaseManager>  dbManager_;

    QTreeView*  treeView_   = nullptr;
    QChartView* chartView_  = nullptr;
    QComboBox*  chartCombo_ = nullptr;
    QComboBox*  styleCombo_ = nullptr;
    QString     currentSource_;
};

} // namespace gui

#endif // GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
