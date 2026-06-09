/// @file parser_registry.cpp
/// @brief Определение реестра парсеров
/// @author Artemenko Anton

#include "parser_registry.hpp"

#include <algorithm>
#include <logger/logger_macros.hpp>

namespace parser
{

void ParserRegistry::Register(const std::string& extension, std::shared_ptr<IParser> parser)
{
    std::string key = extension;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    LogDebug(logger_) << "Parser registered for extension: " << key;
    parsers_[key] = std::move(parser);
}

std::shared_ptr<IParser> ParserRegistry::Get(const std::string& extension) const
{
    auto it = parsers_.find(extension);
    if (it == parsers_.end())
    {
        LogDebug(logger_) << "Parser resolution miss for extension: " << extension;
        return nullptr;
    }
    LogDebug(logger_) << "Parser resolved for extension: " << extension;
    return it->second;
}

std::vector<std::string> ParserRegistry::SupportedExtensions() const
{
    std::vector<std::string> exts;
    exts.reserve(parsers_.size());
    for (const auto& kv : parsers_) exts.push_back(kv.first);
    return exts;
}

}  // namespace parser
