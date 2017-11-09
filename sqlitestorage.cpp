#include "sqlitestorage.h"
#include "sqlitetable.h"

using namespace sqlite;

SQLiteStorage::SQLiteStorage(std::string path)
{
    dbPath = std::move(path);
}

SQLiteStorage::~SQLiteStorage() noexcept
{
    if (mDb != nullptr) {
        sqlite3_close(mDb);
    }
}


bool SQLiteStorage::open()
{
    auto r = sqlite3_open(dbPath.c_str(), &mDb);
    if (r != SQLITE_OK) {
        throw SQLiteException(mDb);
    }
    return true;
}

bool SQLiteStorage::close()
{
    sqlite3_close(mDb);
    mDb = nullptr;
    return true;
}

sqlite3 *SQLiteStorage::handle()
{
    return mDb;
}
