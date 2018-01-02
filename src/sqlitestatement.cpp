//
// Created by Federico Fuga on 16/12/17.
//

#include "sqlitestatement.h"

using namespace sqlite;

struct SQLiteStatement::Impl {
    std::weak_ptr<SQLiteStorage> mDb;
    sqlite3_stmt *stmt = nullptr;
};

SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db,
                                 const sqlite::statements::StatementFormatter &stmt)
{
    init(db);
    prepare(stmt.string());
}

SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db, std::string sql)
{
    init(db);
    prepare(sql);
}

SQLiteStatement::SQLiteStatement(std::shared_ptr<SQLiteStorage> db, const char *sql)
{
    init(db);
    prepare(std::string(sql));
}

SQLiteStatement::~SQLiteStatement()
{
    if (p->stmt != nullptr)
        sqlite3_finalize(p->stmt);
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
        throw SQLiteException(db->handle());
}

void SQLiteStatement::bind(size_t idx, std::string value)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_bind_text(p->stmt, idx, value.c_str(), value.length(), SQLITE_TRANSIENT);
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

bool SQLiteStatement::executeStep(std::function<bool()> func)
{
    auto db = p->mDb.lock();
    auto r = sqlite3_step(p->stmt);
    if (r == SQLITE_DONE) {
        return false;
    } else if (r != SQLITE_ROW) {
        SQLiteException::throwIfNotOk(r, db->handle());
    }

    return func();
}

bool SQLiteStatement::executeStep()
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

int SQLiteStatement::columnCount()
{
    return sqlite3_column_count(p->stmt);
}

bool SQLiteStatement::execute(std::function<bool()> function)
{
    while (executeStep(function));
    sqlite3_reset(p->stmt);
    return true;
}

bool SQLiteStatement::execute()
{
    while (executeStep());
    sqlite3_reset(p->stmt);
    return true;
}
