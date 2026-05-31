#ifndef GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
#define GUID_c9d0e1f2_a3b4_5678_cdef_789012345678

#include <QMainWindow>
#include <QTreeView>
#include <QComboBox>
#include <QtCharts/QChartView>
#include <chart/ichart_builder.hpp>
#include <style/ichart_style.hpp>
#include <parser/iparser_registry.hpp>
#include <map>
#include <functional>
#include <string>

QT_CHARTS_USE_NAMESPACE

namespace gui {

using BuilderFactory = std::map<std::string, std::function<std::shared_ptr<chart::IChartBuilder>()>>;
using StyleFactory   = std::map<std::string, std::function<std::shared_ptr<style::IChartStyle>()>>;

/// @brief Главное окно приложения.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(
        BuilderFactory                           builders,
        StyleFactory                             styles,
        std::shared_ptr<parser::IParserRegistry> registry,
        QWidget* parent = nullptr
    );
    ~MainWindow() override = default;

private slots:
    void onFileSelected(const QModelIndex& index);
    void onSavePdf();

private:
    void loadFile(const QString& path);

    BuilderFactory                           builders_;
    StyleFactory                             styles_;
    std::shared_ptr<parser::IParserRegistry> registry_;

    QTreeView*   treeView_   = nullptr;
    QChartView*  chartView_  = nullptr;
    QComboBox*   chartCombo_ = nullptr;
    QComboBox*   styleCombo_ = nullptr;
};

} // namespace gui

#endif // GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
