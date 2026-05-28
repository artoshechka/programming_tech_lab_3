/// @file sqlite_db.hpp
/// @brief Объявление класса SqliteDB
/// @author Artemenko Anton

#ifndef GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d
#define GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d

#include <database_module/idatabase.hpp>

namespace database
{
class SqliteDB : public IDatabase
{
   public:
    virtual ~SqliteDB() = default;
};
}  // namespace database
#endif  // GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d