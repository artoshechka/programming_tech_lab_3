/// @file ichart_builder.hpp
/// @brief Объявление интерфейса построителя графика из временного ряда.
/// @author Artemenko Anton
#ifndef GUID_a1b2c3d4_e5f6_7890_abcd_ef1234567890
#define GUID_a1b2c3d4_e5f6_7890_abcd_ef1234567890

#include <data_model/src/timeline_data.hpp>
#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

namespace chart {

/// @brief Интерфейс построителя графика из временного ряда.
class IChartBuilder {
public:
    /// @brief Виртуальный деструктор.
    virtual ~IChartBuilder() = default;

    /// @brief Строит и возвращает QChart на основе данных.
    /// @param[in] data Входные данные временного ряда.
    /// @return Указатель на построенный QChart (владение передаётся вызывающему).
    virtual QChart* Build(const data::TimelineData& data) = 0;
};

} // namespace chart

#endif // GUID_a1b2c3d4_e5f6_7890_abcd_ef1234567890
