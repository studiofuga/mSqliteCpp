#include "sqlitestorage.h"
#include "sqlitetable.h"

#include "sqlitestatement.h"

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
    auto r = sqlite3_open_v2(dbPath.c_str(), &mDb, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (r != SQLITE_OK) {
        throw SQLiteException(mDb);
    }
    sqlite3_busy_timeout(mDb, 1000);

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

bool SQLiteStorage::dropTable(std::string table)
{
    std::ostringstream ss;
    ss << "DROP TABLE " << table << ";";
    auto r = sqlite3_exec(mDb, ss.str().c_str(), nullptr, nullptr, nullptr);
    if (r != SQLITE_OK)
        throw SQLiteException(mDb);

    return true;
}

bool SQLiteStorage::tableExists(std::string table)
{
    SQLiteStatement stmt(shared_from_this(), "SELECT name FROM sqlite_master WHERE type='table' AND name=?;");
    stmt.bind(1, table);

    bool found = false;
    stmt.executeStep([&found] {
        found = true;
        return true;
    });
    return found;
}

bool SQLiteStorage::startTransaction()
{
    std::unique_lock<std::mutex> l(mMutex);
    if (mOnTransaction)
        return false;

    auto r = sqlite3_exec(mDb, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    SQLiteException::throwIfNotOk(r, mDb);
    mOnTransaction = true;
    return true;
}

bool SQLiteStorage::commitTransaction()
{
    std::unique_lock<std::mutex> l(mMutex);
    if (!mOnTransaction)
        return false;

    auto r = sqlite3_exec(mDb, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr);
    SQLiteException::throwIfNotOk(r, mDb);
    mOnTransaction = false;
    return true;
}

bool SQLiteStorage::abortTransaction()
{
    std::unique_lock<std::mutex> l(mMutex);
    if (!mOnTransaction)
        return false;

    auto r = sqlite3_exec(mDb, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
    SQLiteException::throwIfNotOk(r, mDb);
    mOnTransaction = false;
    return true;
}

