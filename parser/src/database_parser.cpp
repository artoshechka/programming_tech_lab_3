/// @file database_parser.cpp
/// @brief Определение парсера базы данных
/// @author Artemenko Anton

#include <algorithm>
#include <atomic>
#include <data_model/schema.hpp>
#include <logger/logger_macros.hpp>
#include <parser/parse_exception.hpp>
#include <parser/src/database_parser.hpp>
#include <string>
#include <type_traits>

using data::TimelineData;
using data::TimePoint;
using parser::DatabaseParser;
using parser::ParseException;

namespace
{
/// @brief Разбивает source "path|table" на путь и имя таблицы.
std::pair<std::string, std::string> SplitSource(const std::string& source)
{
    const auto pos = source.find('|');
    if (pos == std::string::npos) return {source, {}};
    return {source.substr(0, pos), source.substr(pos + 1)};
}

/// @brief Экранирует двойные кавычки в SQL-идентификаторе (для безопасной интерполяции).
std::string QuoteIdent(const std::string& name)
{
    std::string s;
    s.reserve(name.size());
    for (char c : name)
    {
        s += c;
        if (c == '"') s += '"';
    }
    return s;
}
}  // namespace

std::atomic<int> DatabaseParser::nextConnId_{0};

TimelineData DatabaseParser::Load(const std::string& source)
{
    LogInfo(logger_) << "DB load: " << source;

    const auto [path, requestedTable] = SplitSource(source);
    const std::string connName = "DBParser_" + std::to_string(nextConnId_++);

    // Соединение закроется автоматически в ~SqliteDB при выходе db из scope —
    // в том числе при пробросе исключения из Query() или ниже по коду.
    auto db = manager_->Create(connName);
    if (!db->Open(path)) throw ParseException("DB: cannot open: " + path);

    const auto tables = db->Tables();
    if (tables.empty()) throw ParseException("DB: no tables in: " + path);

    // Имя таблицы — недоверенный ввод (часть source). Разрешаем только реально
    // существующую таблицу: это закрывает SQL-инъекцию через интерполяцию имени.
    std::string table;
    if (requestedTable.empty())
        table = tables.front();
    else if (std::find(tables.begin(), tables.end(), requestedTable) != tables.end())
        table = requestedTable;
    else
        throw ParseException("DB: no such table: " + requestedTable);

    TimelineData result;
    result.name_ = table;

    // Визитор вызывается для каждой (col, val) в строке последовательно.
    // Начало новой строки детектируем по первой схемной колонке "Time".
    const std::string firstCol = "Time";
    db->Query("SELECT * FROM \"" + QuoteIdent(table) + "\"", [&](const std::string& col, const std::string& val) {
        if (col == firstCol) result.points_.push_back({});
        if (result.points_.empty()) return;
        TimePoint& pt = result.points_.back();
        data::ForEachField(pt, [&](auto& field, const char* name) {
            if (col != name) return;
            if constexpr (std::is_same_v<std::decay_t<decltype(field)>, std::string>)
                field = val;
            else
                field = val.empty() ? 0.0 : std::stod(val);
        });
    });

    LogInfo(logger_) << "DB load done: '" << result.name_ << "', " << result.points_.size() << " points";
    return result;
}
