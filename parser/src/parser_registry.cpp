#include "parser_registry.hpp"

namespace parser {

void ParserRegistry::Register(const std::string& extension, std::shared_ptr<IParser> parser) {
    parsers_[extension] = std::move(parser);
}

std::shared_ptr<IParser> ParserRegistry::Get(const std::string& extension) const {
    auto it = parsers_.find(extension);
    return it != parsers_.end() ? it->second : nullptr;
}

} // namespace parser
