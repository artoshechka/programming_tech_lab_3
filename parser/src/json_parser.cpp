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
#include <parser/parse_exception.hpp>
#include <parser/src/json_parser.hpp>
#include <parser/src/parser_limits.hpp>
#include <string>
#include <type_traits>

using data::TimelineData;
using parser::JsonParser;
using parser::ParseException;

namespace
{
template <typename T>
QJsonValue FieldToJson(const T& field);

template <typename T>
QJsonObject StructToJson(const T& object)
{
    QJsonObject json;
    data::ForEachField(
        object, [&](const auto& value, const char* name) { json[QString::fromUtf8(name)] = FieldToJson(value); });
    return json;
}

template <typename T>
QJsonValue FieldToJson(const T& field)
{
    if constexpr (data::HasSchemaV<T>)
        return StructToJson(field);
    else if constexpr (data::IsVectorV<T>)
    {
        QJsonArray array;
        for (const auto& item : field) array.append(FieldToJson(item));
        return array;
    } else if constexpr (std::is_same_v<T, std::string>)
        return QJsonValue(QString::fromStdString(field));
    else
        return QJsonValue(field);
}

template <typename T>
void JsonToField(T& field, const QJsonValue& json, const char* name);

template <typename T>
void StructFromJson(T& object, const QJsonObject& json)
{
    data::ForEachField(object, [&](auto& value, const char* name) {
        const QJsonValue jsonValue = json.value(QString::fromUtf8(name));
        if (jsonValue.isUndefined()) throw ParseException(std::string("JSON: missing field: ") + name);
        JsonToField(value, jsonValue, name);
    });
}

template <typename T>
void JsonToField(T& field, const QJsonValue& json, const char* name)
{
    if constexpr (data::HasSchemaV<T>)
    {
        if (!json.isObject()) throw ParseException(std::string("JSON: field '") + name + "' is not an object");
        StructFromJson(field, json.toObject());
    } else if constexpr (data::IsVectorV<T>)
    {
        if (!json.isArray()) throw ParseException(std::string("JSON: field '") + name + "' is not an array");
        field.clear();
        const QJsonArray array = json.toArray();
        for (const QJsonValue& item : array)
        {
            typename T::value_type element;
            JsonToField(element, item, name);
            field.push_back(std::move(element));
        }
    } else if constexpr (std::is_same_v<T, std::string>)
    {
        if (!json.isString()) throw ParseException(std::string("JSON: field '") + name + "' is not a string");
        field = json.toString().toStdString();
    } else
    {
        if (!json.isDouble()) throw ParseException(std::string("JSON: field '") + name + "' is not a number");
        field = static_cast<T>(json.toDouble());
    }
}
}  // namespace

TimelineData JsonParser::Load(const std::string& source)
{
    LogInfo(logger_) << "JSON load: " << source;

    QFile file(QString::fromStdString(source));
    if (!file.open(QIODevice::ReadOnly)) throw ParseException("JSON: cannot open file: " + source);

    if (file.size() > kMaxJsonBytes) throw ParseException("JSON: file too large: " + source);

    const QByteArray raw = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject()) throw ParseException("JSON: root is not an object: " + source);

    TimelineData result;
    StructFromJson(result, doc.object());
    LogInfo(logger_) << "JSON load done: '" << result.name_ << "', " << result.points_.size() << " points";
    return result;
}
