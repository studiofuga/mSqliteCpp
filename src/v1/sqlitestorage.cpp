
#include "msqlitecpp/v1/sqlitestorage.h"

#include "msqlitecpp/v1/sqlitestorage.h"
#include "msqlitecpp/v1/sqlitetable.h"

#include "msqlitecpp/v1/sqlitestatement.h"

#include "utils/make_unique.h"

using namespace sqlite;

SQLiteStorage::SQLiteStorage(std::string path)
{
    dbPath = std::move(path);
}

SQLiteStorage::~SQLiteStorage() noexcept
{
    if (mDb != nullptr) {
        // Finalize all external active statements before closing the database
        // (the internal transaction statements will be finalized by their destructors)
        {
            std::unique_lock<std::mutex> l(mMutex);
            for (auto* stmt : mActiveStatements) {
                if (stmt != nullptr) {
                    sqlite3_finalize(stmt);
                }
            }
            mActiveStatements.clear();
        }

        // Now reset transaction statements (they won't be in mActiveStatements)
        mBeginTransaction.reset();
        mCommitTransaction.reset();
        mAbortTransaction.reset();

        // Finally close the database with close_v2
        sqlite3_close_v2(mDb);
        mDb = nullptr;
    }
}


bool SQLiteStorage::open()
{
    auto r = sqlite3_open_v2(dbPath.c_str(), &mDb, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (r != SQLITE_OK) {
        throw SQLiteException(mDb);
    }
    sqlite3_busy_timeout(mDb, 1000);

    mBeginTransaction = utils::make_unique<SQLiteStatement>(shared_from_this(), "BEGIN TRANSACTION;");
    mCommitTransaction = utils::make_unique<SQLiteStatement>(shared_from_this(), "COMMIT TRANSACTION;");
    mAbortTransaction = utils::make_unique<SQLiteStatement>(shared_from_this(), "ROLLBACK TRANSACTION;");

    // Disable tracking for internal transaction statements
    mBeginTransaction->disableTracking();
    mCommitTransaction->disableTracking();
    mAbortTransaction->disableTracking();

    for (auto &flag : mFlags) {
        switch (flag) {
            case Flags ::EnforceForeignKeys:
                r = sqlite3_exec(mDb, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
                if (r != SQLITE_OK)
                    throw SQLiteException(mDb);
                break;
            default:
                throw std::logic_error("Unhandled case for flag");
        }
    }

    return true;
}

bool SQLiteStorage::close()
{
    if (mDb != nullptr) {
        // Finalize all external active statements before closing the database
        {
            std::unique_lock<std::mutex> l(mMutex);
            for (auto* stmt : mActiveStatements) {
                if (stmt != nullptr) {
                    sqlite3_finalize(stmt);
                }
            }
            mActiveStatements.clear();
        }

        // Reset transaction statements
        mBeginTransaction.reset();
        mCommitTransaction.reset();
        mAbortTransaction.reset();

        // Finally close the database with close_v2
        sqlite3_close_v2(mDb);
        mDb = nullptr;
    }
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

    mBeginTransaction->execute();
    mOnTransaction = true;
    return true;
}

bool SQLiteStorage::commitTransaction()
{
    std::unique_lock<std::mutex> l(mMutex);
    if (!mOnTransaction)
        return false;

    mCommitTransaction->execute();
    mOnTransaction = false;
    return true;
}

bool SQLiteStorage::abortTransaction()
{
    std::unique_lock<std::mutex> l(mMutex);
    if (!mOnTransaction)
        return false;

    mAbortTransaction->execute();
    mOnTransaction = false;
    return true;
}

size_t SQLiteStorage::getLastRowId()
{
    return sqlite3_last_insert_rowid(handle());
}

void SQLiteStorage::setFlag(SQLiteStorage::Flags flag)
{
    mFlags.insert(flag);
}

void SQLiteStorage::registerStatement(sqlite3_stmt* stmt, bool trackForCleanup)
{
    if (stmt != nullptr && trackForCleanup) {
        std::unique_lock<std::mutex> l(mMutex);
        mActiveStatements.insert(stmt);
    }
}

void SQLiteStorage::unregisterStatement(sqlite3_stmt* stmt)
{
    if (stmt != nullptr) {
        std::unique_lock<std::mutex> l(mMutex);
        mActiveStatements.erase(stmt);
    }
}
