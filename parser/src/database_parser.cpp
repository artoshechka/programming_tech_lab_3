/// @file database_parser.cpp
/// @brief Определение парсера базы данных
/// @author Artemenko Anton

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <data_model/schema.hpp>
#include <logger/logger_macros.hpp>
#include <parser/src/database_parser.hpp>
#include <string>
#include <type_traits>

using data::TimelineData;
using data::TimePoint;
using parser::DatabaseParser;

namespace
{
/// @brief Уникальное имя соединения, чтобы не пересекаться между вызовами.
const char* const kConnectionName = "DatabaseParserConnection";

/// @brief Заполняет одно поле точки из значения колонки QVariant.
/// @tparam T Тип поля.
/// @param[out] field Заполняемое поле.
/// @param[in] value Значение колонки.
template <typename T>
void FieldFromVariant(T& field, const QVariant& value)
{
    if constexpr (std::is_same_v<T, std::string>)
    {
        field = value.toString().toStdString();
    } else
    {
        field = static_cast<T>(value.toDouble());
    }
}

/// @brief Считывает текущую строку запроса в TimePoint по строковым именам схемы.
/// @param[in] query Запрос, спозиционированный на строке.
/// @return Заполненная точка ряда.
TimePoint ReadPoint(const QSqlQuery& query)
{
    TimePoint point;
    data::ForEachField(
        point, [&](auto& value, const char* name) { FieldFromVariant(value, query.value(QString::fromUtf8(name))); });
    return point;
}
}  // namespace
TimelineData DatabaseParser::Load(const std::string& source)
{
    LogInfo(logger_) << "DB load started: " << source;

    TimelineData result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", kConnectionName);
        db.setDatabaseName(QString::fromStdString(source));
        if (db.open())
        {
            const QStringList tables = db.tables();
            if (!tables.isEmpty())
            {
                const QString table = tables.first();
                result.name_ = table.toStdString();
                LogDebug(logger_) << "DB table selected: " << result.name_;

                QSqlQuery query(db);
                query.exec(QString("SELECT * FROM \"%1\"").arg(table));
                while (query.next())
                {
                    result.points_.push_back(ReadPoint(query));
                }
            } else
            {
                LogWarning(logger_) << "DB load: no tables in " << source;
            }
            db.close();
        } else
        {
            LogError(logger_) << "DB load failed: cannot open " << source;
        }
    }
    QSqlDatabase::removeDatabase(kConnectionName);

    LogInfo(logger_) << "DB load finished: series '" << result.name_ << "', " << result.points_.size() << " points";
    return result;
}

void DatabaseParser::Save(const TimelineData& data, const std::string& destination)
{
    LogInfo(logger_) << "DB save started: series '" << data.name_ << "', " << data.points_.size() << " points -> "
                     << destination;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", kConnectionName);
        db.setDatabaseName(QString::fromStdString(destination));
        if (db.open())
        {
            const QString table = QString::fromStdString(data.name_);
            QSqlQuery query(db);
            query.exec(QString("DROP TABLE IF EXISTS \"%1\"").arg(table));
            query.exec(QString("CREATE TABLE \"%1\" ([Time] Datetime, [Value] Float)").arg(table));

            db.transaction();
            query.prepare(QString("INSERT INTO \"%1\" ([Time], [Value]) VALUES (?, ?)").arg(table));
            for (const TimePoint& point : data.points_)
            {
                query.addBindValue(QString::fromStdString(point.time_));
                query.addBindValue(point.value_);
                query.exec();
            }
            db.commit();
            db.close();
            LogInfo(logger_) << "DB save finished: " << destination;
        } else
        {
            LogError(logger_) << "DB save failed: cannot open " << destination;
        }
    }
    QSqlDatabase::removeDatabase(kConnectionName);
}
