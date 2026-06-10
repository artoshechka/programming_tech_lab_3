/// @file chart_model.cpp
/// @brief Определение ChartModel
/// @author Artemenko Anton

#include <gui/src/chart_model.hpp>
#include <logger/logger_macros.hpp>
#include <parser/iparser.hpp>
#include <parser/parse_exception.hpp>
#include <exception>
#include <system_error>
#include <utility>

namespace gui
{

ChartModel::ChartModel(std::shared_ptr<parser::IParserRegistry> registry, std::shared_ptr<logger::ILogger> logger,
                       QObject* parent)
    : QObject(parent), registry_(std::move(registry)), logger_(std::move(logger))
{
}

const data::TimelineData& ChartModel::data() const
{
    // Контракт: вызывать только при hasData(); value() бросит при пустом слоте — явный сигнал об ошибке вызова.
    return dataSlot_.value().data;
}

std::vector<std::string> ChartModel::listSubSources(const std::string& path)
{
    // Инвалидация по mtime: пока путь и время модификации совпадают — отдаём из слота, файл не читаем.
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(path, ec);
    if (!ec && tablesSlot_ && tablesSlot_->path == path && tablesSlot_->mtime == mtime)
    {
        LogDebug(logger_) << "Sub-sources cache hit: " << path << " (" << tablesSlot_->tables.size() << ")";
        return tablesSlot_->tables;
    }

    auto ext = path.substr(path.rfind('.') + 1);
    for (auto& c : ext) c = static_cast<char>(::tolower(c));
    auto parser = registry_->Get(ext);
    if (!parser) return {};

    LogDebug(logger_) << "Sub-sources cache miss: " << path << ", probing via parser '" << ext << "'";
    auto tables = parser->ListSubSources(path);
    LogInfo(logger_) << "Sub-sources listed: " << path << " -> " << tables.size();

    // Перечитали — затираем слот свежим значением (если удалось получить mtime).
    if (!ec) tablesSlot_ = TablesSlot{path, mtime, tables};
    return tables;
}

void ChartModel::setSource(const std::string& source)
{
    const std::string path = SourcePath(source);

    // Проверяем mtime файла-источника для инвалидации кэша при внешних изменениях.
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(path, ec);
    if (ec)
    {
        LogError(logger_) << "Cannot stat source: " << path << ": " << ec.message();
        emit errorOccurred(QString::fromStdString("Cannot stat source: " + path + ": " + ec.message()));
        return;
    }

    // Совпали source и mtime — данные не перечитываем, но всё равно уведомляем View об (повторном) выборе.
    if (!dataSlot_ || dataSlot_->source != source || dataSlot_->mtime != mtime)
    {
        LogDebug(logger_) << "Data cache miss: " << source << " (re-parsing)";

        auto ext = path.substr(path.rfind('.') + 1);
        for (auto& c : ext) c = static_cast<char>(::tolower(c));

        auto parser = registry_->Get(ext);
        if (!parser)
        {
            LogError(logger_) << "Unknown format: " << ext << " for source: " << source;
            emit errorOccurred(QString::fromStdString("Unknown format: " + ext));
            return;
        }

        try
        {
            // parser->Load бросает ParseException — ловим и уведомляем сигналом, старый слот не трогаем.
            dataSlot_ = DataSlot{source, mtime, parser->Load(source)};
        } catch (const std::exception& e)
        {
            LogError(logger_) << "Load failed for '" << source << "': " << e.what();
            emit errorOccurred(QString::fromUtf8(e.what()));
            return;
        }
        LogInfo(logger_) << "Source loaded: " << source << " (" << dataSlot_->data.points_.size() << " points)";
    } else
    {
        LogDebug(logger_) << "Data cache hit: " << source << " (no re-parse)";
    }

    emit dataChanged();
}

void ChartModel::setBuilder(const std::string& builder)
{
    if (builder_ == builder) return;
    builder_ = builder;
    emit renderOptionsChanged();
}

void ChartModel::setStyle(const std::string& style)
{
    if (style_ == style) return;
    style_ = style;
    emit renderOptionsChanged();
}

void ChartModel::setAggregate(bool aggregate)
{
    if (aggregate_ == aggregate) return;
    aggregate_ = aggregate;
    emit renderOptionsChanged();
}

std::string ChartModel::SourcePath(const std::string& source)
{
    const auto pos = source.find('|');
    return pos == std::string::npos ? source : source.substr(0, pos);
}

}  // namespace gui
