/// @file sqlite_db_manager.hpp
/// @brief Объявление класса SqliteDBManager
/// @author Artemenko Anton

#ifndef GUID_5551891a_c619_4650_901b_b7244fbe8b95
#define GUID_5551891a_c619_4650_901b_b7244fbe8b95

#include <database_module/idatabase_manager.hpp>

namespace database::manager
{
class SqliteDBManager : public IDatabaseManager
{
   public:
    virtual ~SqliteDBManager() = default;
};
}  // namespace database::manager

#endif  // GUID_5551891a_c619_4650_901b_b7244fbe8b95