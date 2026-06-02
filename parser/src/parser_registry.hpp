#ifndef GUID_b8c9d0e1_f2a3_4567_bcde_678901234567
#define GUID_b8c9d0e1_f2a3_4567_bcde_678901234567

#include <parser/iparser_registry.hpp>
#include <map>

namespace parser {

/// @brief Реестр парсеров: extension → IParser.
class ParserRegistry : public IParserRegistry {
public:
    void Register(const std::string& extension, std::shared_ptr<IParser> parser);
    std::shared_ptr<IParser> Get(const std::string& extension) const override;
private:
    std::map<std::string, std::shared_ptr<IParser>> parsers_;
};

} // namespace parser

#endif // GUID_b8c9d0e1_f2a3_4567_bcde_678901234567
