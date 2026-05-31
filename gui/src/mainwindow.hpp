#ifndef GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
#define GUID_c9d0e1f2_a3b4_5678_cdef_789012345678

#include <QMainWindow>
#include <memory>
#include <chart/ichart_builder.hpp>
#include <style/ichart_style.hpp>
#include <parser/iparser_registry.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace gui {

/// @brief Главное окно приложения.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(
        std::shared_ptr<chart::IChartBuilder> builder,
        std::shared_ptr<style::IChartStyle>   style,
        std::shared_ptr<parser::IParserRegistry> registry,
        QWidget* parent = nullptr
    );
    ~MainWindow() override;

private slots:
    void onOpenFile();
    void onSavePdf();

private:
    void loadFile(const QString& path);

    Ui::MainWindow* ui_;
    std::shared_ptr<chart::IChartBuilder>      builder_;
    std::shared_ptr<style::IChartStyle>        style_;
    std::shared_ptr<parser::IParserRegistry>   registry_;
    QChartView* chartView_ = nullptr;
};

} // namespace gui

#endif // GUID_c9d0e1f2_a3b4_5678_cdef_789012345678
