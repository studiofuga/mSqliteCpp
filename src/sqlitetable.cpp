#include "sqlitetable.h"

#include "sqlitestorage.h"
#include <sqlite3.h>

#include <sstream>
#include <iostream>

using namespace sqlite;

class SQLiteTable::Statement {
    std::shared_ptr<SQLiteStorage> mDb;
    sqlite3_stmt *stmt = nullptr;
public:
    explicit Statement(std::shared_ptr<SQLiteStorage> db, std::string sql)
    : mDb(db)
    {
        auto r = sqlite3_prepare_v2(mDb->handle(), sql.c_str(), -1, &stmt, nullptr);
        if (r != SQLITE_OK)
            throw SQLiteException(mDb->handle());
    }

    ~Statement() {
        if (stmt != nullptr)
            sqlite3_finalize(stmt);
    }

    sqlite3_stmt *handle() const { return stmt; }
    std::shared_ptr<SQLiteStorage> db() const { return mDb; }
};

SQLiteTable::SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : mdb(db), mName(name)
{

}

std::shared_ptr<SQLiteStorage> SQLiteTable::db()
{
    auto db = mdb.lock();

    if (db == nullptr)
        throw std::logic_error("Operation on an orphaned Table!");
    return db;
}

bool SQLiteTable::createFromSQLString(std::string query)
{
    auto db = this->db();

    sqlite3_stmt *s;
    if (sqlite3_prepare(db->handle(), query.c_str(), query.size(), &s, nullptr) != SQLITE_OK)
        throw SQLiteException(db->handle());

    if (sqlite3_step(s) != SQLITE_DONE) {
        throw SQLiteException(db->handle());
    }

    return true;
}

template<>
void SQLiteTable::bindValue(SQLiteTable::Statement *stmt, int idx, int value)
{
    auto r = sqlite3_bind_int(stmt->handle(), idx, value);
    if (r != SQLITE_OK)
        throw SQLiteException(stmt->db()->handle());
}

template<>
void SQLiteTable::getValue(SQLiteTable::Statement *stmt, int idx, int &value)
{
    if (sqlite3_column_type(stmt->handle(), idx) != SQLITE_INTEGER)
        throw std::runtime_error("Not a SQLITE_INTEGER type column");

    value = sqlite3_column_int(stmt->handle(), idx);
}

template<>
void SQLiteTable::bindValue(SQLiteTable::Statement *stmt, int idx, std::string value)
{
    auto r = sqlite3_bind_text(stmt->handle(), idx, value.c_str(), value.size(), SQLITE_TRANSIENT);
    if (r != SQLITE_OK)
        throw SQLiteException(stmt->db()->handle());
}

template<>
void SQLiteTable::getValue(SQLiteTable::Statement *stmt, int idx, std::string &value)
{
    if (sqlite3_column_type(stmt->handle(), idx) != SQLITE_TEXT)
        throw std::runtime_error("Not a SQLITE_TEXT type column");

    auto sptr = sqlite3_column_text(stmt->handle(), idx);
    auto len = sqlite3_column_bytes(stmt->handle(), idx);
    value = std::string(sptr, sptr + len);
}

template<>
void SQLiteTable::bindValue(SQLiteTable::Statement *stmt, int idx, double value)
{
    auto r = sqlite3_bind_double(stmt->handle(), idx, value);
    if (r != SQLITE_OK)
        throw SQLiteException(stmt->db()->handle());
}

std::shared_ptr<SQLiteTable::Statement> SQLiteTable::newStatement(std::string query)
{
    auto s = std::make_shared<Statement>(mdb.lock(), query);

    return s;
}

bool SQLiteTable::execute(SQLiteTable::Statement *stmt)
{
    auto r = sqlite3_step(stmt->handle());

    if (r != SQLITE_ROW && r != SQLITE_DONE)
        throw SQLiteException(stmt->db()->handle());
    return true;
}

bool SQLiteTable::hasData(Statement *stmt) const
{
    auto r = sqlite3_step(stmt->handle());
    if (r != SQLITE_DONE && r != SQLITE_ROW)
        SQLiteException::throwIfNotOk(r, stmt->db()->handle());

    return r == SQLITE_ROW;
}

int SQLiteTable::columnCount(Statement *stmt) const
{
    return sqlite3_column_count(stmt->handle());
}

SQLiteTable::~SQLiteTable() noexcept = default;

