//
// Created by Federico Fuga on 16/12/17.
//

#include "sqlitestatement.h"

using namespace sqlite;

struct SQLiteStatement::Impl {
    std::weak_ptr<SQLiteStorage> mDb;
    sqlite3_stmt *stmt = nullptr;
};

SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db, std::string sql)
        : p(std::make_unique<Impl>())
{
    p->mDb = db;
    auto r = sqlite3_prepare_v2(db->handle(), sql.c_str(), -1, &p->stmt, nullptr);
    if (r != SQLITE_OK)
        throw SQLiteException(db->handle());
}

SQLiteStatement::~SQLiteStatement()
{
    if (p->stmt != nullptr)
        sqlite3_finalize(p->stmt);
}

void SQLiteStatement::bind(int idx, std::string value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_text(p->stmt, 1, value.c_str(), value.length(), SQLITE_TRANSIENT);
    SQLiteException::throwIfNotOk(r,db->handle());
}

bool SQLiteStatement::execute(std::function<bool()> func)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_step(p->stmt);
    if (r == SQLITE_DONE) {
        return true;
    } else if (r != SQLITE_ROW) {
        SQLiteException::throwIfNotOk(r, db->handle());
    }

    return func();
}
