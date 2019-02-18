//
// Created by Federico Fuga on 16/12/17.
//

#include "sqlitestatement.h"

using namespace sqlite;

struct sqlite::SQLiteStatement::Impl {
    std::weak_ptr<SQLiteStorage> mDb;
    sqlite3_stmt *stmt = nullptr;
};


sqlite::SQLiteStatement::SQLiteStatement()
{

}

sqlite::SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db,
                                 const sqlite::statements::StatementFormatter &stmt)
{
    attach(db,stmt);
}

sqlite::SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db, std::string sql)
{
    attach(db,sql);
}

sqlite::SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db, const char *sql)
{
    attach(db, sql);
}

sqlite::SQLiteStatement::~SQLiteStatement()
{
    if (p != nullptr && p->stmt != nullptr)
        sqlite3_finalize(p->stmt);
}

sqlite::SQLiteStatement::SQLiteStatement(SQLiteStatement &&) = default;
sqlite::SQLiteStatement &SQLiteStatement::operator =(SQLiteStatement &&) = default;

void SQLiteStatement::attach(std::shared_ptr<SQLiteStorage> dbm)
{
    init(dbm);
}

void sqlite::SQLiteStatement::attach(std::shared_ptr<SQLiteStorage> dbm, std::string stmt)
{
    init(dbm);
    prepare(std::string(stmt));
}

void
sqlite::SQLiteStatement::attach(std::shared_ptr<SQLiteStorage> db, const sqlite::statements::StatementFormatter &stmt)
{
    init(db);
    prepare(stmt.string());
}

void SQLiteStatement::prepare(const sqlite::statements::StatementFormatter &stmt)
{
    try {
        prepare(stmt.string());
    } catch (sqlite::SQLiteException &x) {
        std::ostringstream ss;
        ss << x.what() << ": " << stmt.string();
        throw sqlite::SQLiteException(x, ss.str());
    }
}

void SQLiteStatement::init(std::shared_ptr<SQLiteStorage> db)
{
    p = (std::make_unique<Impl>());
    p->mDb = db;
}

void SQLiteStatement::prepare(std::string sql)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_prepare_v2(db->handle(), sql.c_str(), -1, &p->stmt, nullptr);
    if (r != SQLITE_OK)
        throw SQLiteException(db->handle(), sql);
}

void SQLiteStatement::bind(size_t idx, std::string value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_text(p->stmt, idx, value.c_str(), value.length(), SQLITE_TRANSIENT);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, unsigned long long value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, long long value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, unsigned long value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, long value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, unsigned int value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_int(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, int value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_int(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, double value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_double(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, float value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_double(p->stmt, idx, value);
    SQLiteException::throwIfNotOk(r,db->handle());
}

void SQLiteStatement::bind(size_t idx, std::nullptr_t value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_null(p->stmt, idx);
    SQLiteException::throwIfNotOk(r,db->handle());
}

long long SQLiteStatement::getLongValue(int idx)
{
    return sqlite3_column_int64(p->stmt, idx);
}

unsigned long long SQLiteStatement::getULongValue(int idx)
{
    return sqlite3_column_int64(p->stmt, idx);
}

int SQLiteStatement::getIntValue(int idx)
{
    return sqlite3_column_int(p->stmt, idx);
}

double SQLiteStatement::getDoubleValue(int idx)
{
    return sqlite3_column_double(p->stmt, idx);
}

std::string SQLiteStatement::getStringValue(int idx)
{
    auto sptr = sqlite3_column_text(p->stmt, idx);
    auto len = sqlite3_column_bytes(p->stmt, idx);
    return std::string(sptr, sptr + len);
}

SQLiteStatement::QueryResult SQLiteStatement::executeStep(std::function<bool()> func)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_step(p->stmt);
    if (r == SQLITE_DONE) {
        return QueryResult::Completed;
    } else if (r != SQLITE_ROW) {
        SQLiteException::throwIfNotOk(r, db->handle());
    }

    return (func() ? QueryResult::Ongoing : QueryResult::Aborted);
}

SQLiteStatement::QueryResult SQLiteStatement::executeStep()
{
    return executeStep([]() { return true; });
}

FieldType::Type SQLiteStatement::columnType(int idx)
{
    switch (sqlite3_column_type(p->stmt, idx)) {
        case SQLITE_TEXT:
            return FieldType::Type::Text;
        case SQLITE_INTEGER:
            return FieldType::Type::Integer;
        case SQLITE_FLOAT:
            return FieldType::Type::Real;
        case SQLITE_BLOB:
            return FieldType::Type::Blob;
    }
    throw std::runtime_error("Unhandled sqlite3 type");
}

bool SQLiteStatement::isNull(int idx)
{
    return sqlite3_column_type(p->stmt, idx) == SQLITE_NULL;
}

int SQLiteStatement::columnCount()
{
    return sqlite3_column_count(p->stmt);
}

bool SQLiteStatement::execute(std::function<bool()> function)
{
    QueryResult result;
    try {
        while ((result = executeStep(function)) == SQLiteStatement::QueryResult::Ongoing);
    } catch (SQLiteException &) {
        sqlite3_reset(p->stmt); // Reset the statement before throwing again
        throw;
    }
    sqlite3_reset(p->stmt);
    sqlite3_clear_bindings(p->stmt);
    return result == SQLiteStatement::QueryResult::Completed;
}

bool SQLiteStatement::execute()
{
    return execute([]() { return true; });
}

