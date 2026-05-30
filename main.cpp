/// @file main.cpp
/// @brief Демонстрационный прогон парсеров с логированием
/// @author Artemenko Anton

#include <array>
#include <logger/logger_factory.hpp>
#include <logger/logger_macros.hpp>
#include <memory>
#include <parser/src/database_parser.hpp>
#include <parser/src/json_parser.hpp>
#include <string>

/// @brief Точка входа: демонстрирует работу парсеров на старых БД и JSON-наборах.
/// @return Код возврата процесса.
int main()
{
    // Одна сущность логгера, далее раздаётся парсерам копией shared_ptr.
    std::shared_ptr<logger::ILogger> log = logger::GetLogger<logger::AppLoggerTag>();
    LogInfo(log) << "Application started";

    parser::JsonParser jsonParser(log);
    parser::DatabaseParser dbParser(log);

    const std::string kRoot = std::string(PROJECT_ROOT_DIR) + "/";
    const std::string kInputDir = kRoot + "test_data/InputData/";
    const std::string kDataDir = kRoot + "test_data/";
    const std::string kOutDir = kRoot + "test_data/output/";

    // Сценарий 1: чтение старых SQLite-баз (только Load, на диск ничего не пишем).
    LogInfo(log) << "=== Scenario 1: read existing SQLite databases ===";
    const std::array<std::string, 5> kDatabases = {"BLOOD_SUGAR", "HUMIDITY_MOSCOW", "NORDPOOL_PRICES",
                                                   "PRICES_NATURAL_GAS_USD", "TEMPERATURE_NOVOSIB"};
    for (const std::string& name : kDatabases)
    {
        dbParser.Load(kInputDir + name + ".sqlite");
    }

    // Сценарий 2: DB -> JSON конвертация (читаем старую базу, пишем только JSON).
    LogInfo(log) << "=== Scenario 2: convert SQLite -> JSON ===";
    data::TimelineData fromDb = dbParser.Load(kInputDir + "TEMPERATURE_NOVOSIB.sqlite");
    jsonParser.Save(fromDb, kOutDir + "TEMPERATURE_NOVOSIB.json");

    // Сценарий 3: JSON round-trip по сгенерированным наборам (Load -> Save в output).
    LogInfo(log) << "=== Scenario 3: JSON round-trip ===";
    const std::array<std::string, 4> kSeries = {"SERIES_10K", "SERIES_20K", "SERIES_35K", "SERIES_50K"};
    for (const std::string& series : kSeries)
    {
        data::TimelineData data = jsonParser.Load(kDataDir + series + ".json");
        jsonParser.Save(data, kOutDir + series + "_roundtrip.json");
    }

    LogInfo(log) << "Application finished";
    return 0;
}
