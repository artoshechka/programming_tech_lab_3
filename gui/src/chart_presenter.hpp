/// @file chart_presenter.hpp
/// @brief Логика загрузки данных и построения графика
/// @author Artemenko Anton
#ifndef GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
#define GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891

#include <gui/src/mainwindow.hpp>
#include <data_model/src/timeline_data.hpp>
#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

namespace gui {

/// @brief Загружает данные, кэширует их и строит QChart по текущим builder/style.
class ChartPresenter {
public:
    ChartPresenter(BuilderFactory builders, StyleFactory styles,
                   std::shared_ptr<parser::IParserRegistry> registry);

    /// @brief Загружает файл, кэширует TimelineData, строит и возвращает QChart.
    /// @throws parser::ParseException при ошибке парсинга.
    QChart* load(const std::string& source, const std::string& builder, const std::string& style);

    /// @brief Пересобирает QChart из кэша (без IO).
    /// @return nullptr если кэш пуст.
    QChart* rebuild(const std::string& builder, const std::string& style);

private:
    QChart* buildChart(const std::string& builder, const std::string& style);

    BuilderFactory builders_;
    StyleFactory   styles_;
    std::shared_ptr<parser::IParserRegistry> registry_;
    data::TimelineData cached_;
    bool hasCached_ = false;
};

} // namespace gui

#endif // GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
