/// @file json_parser.cpp
/// @brief Определение парсера JSON
/// @author Artemenko Anton

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <data_model/schema.hpp>
#include <logger/logger_macros.hpp>
#include <parser/src/json_parser.hpp>
#include <string>
#include <type_traits>

using data::TimelineData;
using parser::JsonParser;

namespace
{
/// @brief Сериализует одно поле в QJsonValue согласно его типу.
/// @tparam T Тип поля.
/// @param[in] field Значение поля.
/// @return JSON-представление поля.
template <typename T>
QJsonValue FieldToJson(const T& field);

/// @brief Сериализует структуру со схемой в QJsonObject.
/// @tparam T Тип структуры (должен иметь kSchema).
/// @param[in] object Сериализуемый объект.
/// @return JSON-объект с полями по строковым именам схемы.
template <typename T>
QJsonObject StructToJson(const T& object)
{
    QJsonObject json;
    data::ForEachField(const_cast<T&>(object), [&](const auto& value, const char* name) {
        json[QString::fromUtf8(name)] = FieldToJson(value);
    });
    return json;
}

template <typename T>
QJsonValue FieldToJson(const T& field)
{
    if constexpr (data::HasSchemaV<T>)
    {
        return StructToJson(field);
    } else if constexpr (data::IsVectorV<T>)
    {
        QJsonArray array;
        for (const auto& item : field)
        {
            array.append(FieldToJson(item));
        }
        return array;
    } else if constexpr (std::is_same_v<T, std::string>)
    {
        return QJsonValue(QString::fromStdString(field));
    } else
    {
        return QJsonValue(field);
    }
}
/// @brief Десериализует одно поле из QJsonValue согласно его типу.
/// @tparam T Тип поля.
/// @param[out] field Заполняемое поле.
/// @param[in] json Источник значения.
template <typename T>
void JsonToField(T& field, const QJsonValue& json);

/// @brief Заполняет структуру со схемой из QJsonObject.
/// @tparam T Тип структуры (должен иметь kSchema).
/// @param[out] object Заполняемый объект.
/// @param[in] json Источник значений по строковым именам схемы.
template <typename T>
void StructFromJson(T& object, const QJsonObject& json)
{
    data::ForEachField(object,
                       [&](auto& value, const char* name) { JsonToField(value, json.value(QString::fromUtf8(name))); });
}

template <typename T>
void JsonToField(T& field, const QJsonValue& json)
{
    if constexpr (data::HasSchemaV<T>)
    {
        StructFromJson(field, json.toObject());
    } else if constexpr (data::IsVectorV<T>)
    {
        field.clear();
        const QJsonArray array = json.toArray();
        for (const QJsonValue& item : array)
        {
            typename T::value_type element;
            JsonToField(element, item);
            field.push_back(std::move(element));
        }
    } else if constexpr (std::is_same_v<T, std::string>)
    {
        field = json.toString().toStdString();
    } else
    {
        field = static_cast<T>(json.toDouble());
    }
}
}  // namespace

TimelineData JsonParser::Load(const std::string& source)
{
    LogInfo(logger_) << "JSON load started: " << source;

    TimelineData result;

    QFile file(QString::fromStdString(source));
    if (!file.open(QIODevice::ReadOnly))
    {
        LogError(logger_) << "JSON load failed: cannot open file " << source;
        return result;
    }

    const QByteArray raw = file.readAll();
    file.close();

    const QJsonDocument document = QJsonDocument::fromJson(raw);
    if (!document.isObject())
    {
        LogError(logger_) << "JSON load failed: root is not an object in " << source;
        return result;
    }

    StructFromJson(result, document.object());
    LogInfo(logger_) << "JSON load finished: series '" << result.name_ << "', " << result.points_.size() << " points";
    return result;
}

void JsonParser::Save(const TimelineData& data, const std::string& destination)
{
    LogInfo(logger_) << "JSON save started: series '" << data.name_ << "', " << data.points_.size() << " points -> "
                     << destination;

    const QJsonObject root = StructToJson(data);
    const QJsonDocument document(root);

    QFile file(QString::fromStdString(destination));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        LogError(logger_) << "JSON save failed: cannot open file " << destination;
        return;
    }

    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    LogInfo(logger_) << "JSON save finished: " << destination;
}
