#ifndef SQLITERESULTSTORAGE_H
#define SQLITERESULTSTORAGE_H

#include "msqlitecpp.h"
#include "sqlite3.h"

#include <string>
#include <memory>
#include <stdexcept>
#include <list>
#include <thread>
#include <mutex>
#include <sstream>
#include <set>

namespace sqlite {
class SQLiteTable;

class SQLiteStatement;

class EXPORT SQLiteException : public std::runtime_error {
    std::string mErrmsg;
    int mCode;
public:
    explicit SQLiteException(sqlite3 *db) : std::runtime_error("")
    {
        mErrmsg = sqlite3_errmsg(db);
        mCode = sqlite3_errcode(db);
    }

    explicit SQLiteException(sqlite3 *db, std::string details) : std::runtime_error("")
    {
        std::ostringstream ss;
        ss << details << ": " << sqlite3_errmsg(db);
        mErrmsg = ss.str();
        mCode = sqlite3_errcode(db);
    }

    explicit SQLiteException(const SQLiteException &x, std::string details) : std::runtime_error("")
    {
        std::ostringstream ss;
        ss << details << ": " << x.what();
        mErrmsg = ss.str();
        mCode = x.code();
    }

    const char *what() const noexcept override
    {
        return mErrmsg.c_str();
    }

    int code() const
    {
        return mCode;
    }

    static void throwIfNotOk(int returnCode, sqlite3 *db)
    {
        if (returnCode != SQLITE_OK) {
            throw SQLiteException(db);
        }
    }
};

class EXPORT SQLiteStorage : public std::enable_shared_from_this<SQLiteStorage> {
public:
    enum class Flags {
        EnforceForeignKeys
    };

private:
    mutable std::mutex mMutex;
    std::string dbPath;

    sqlite3 *mDb = nullptr;

    bool mOnTransaction = false;

    std::unique_ptr<SQLiteStatement> mBeginTransaction, mCommitTransaction, mAbortTransaction;

    std::set<Flags> mFlags;
public:

    explicit SQLiteStorage(std::string path);

    ~SQLiteStorage() noexcept;

    void setFlag(Flags);

    bool open();

    bool close();

    sqlite3 *handle();

    bool dropTable(std::string table);

    bool tableExists(std::string table);

    bool isOngoingTransaction() const
    {
        std::unique_lock<std::mutex> l(mMutex);
        return mOnTransaction;
    }

    bool startTransaction();

    bool commitTransaction();

    bool abortTransaction();

    size_t getLastRowId();
};

} // ns sqlite

#endif // SQLITERESULTSTORAGE_H
