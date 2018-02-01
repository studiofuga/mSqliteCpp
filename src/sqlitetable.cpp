#include "sqlitetable.h"

#include "sqlitestorage.h"
#include <sqlite3.h>

#include <sstream>
#include <iostream>
#include "sqlitestatement.h"

using namespace sqlite;

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
void SQLiteTable::bindValue(SQLiteStatement *stmt, int idx, int value)
{
    stmt->bind(idx, value);
}

template<>
void SQLiteTable::getValue(SQLiteStatement *stmt, int idx, int &value)
{
    if (stmt->columnType(idx) != FieldType::Type::Integer)
        throw std::runtime_error("Not a SQLITE_INTEGER type column");


    value = stmt->getIntValue(idx);
}

template<>
void SQLiteTable::bindValue(SQLiteStatement *stmt, int idx, std::string value)
{
    stmt->bind(idx, value);
}

template<>
void SQLiteTable::getValue(SQLiteStatement *stmt, int idx, std::string &value)
{
    if (stmt->columnType(idx) != FieldType::Type::Text)
        throw std::runtime_error("Not a SQLITE_TEXT type column");

    value = stmt->getStringValue(idx);
}

template<>
void SQLiteTable::bindValue(SQLiteStatement *stmt, int idx, double value)
{
    stmt->bind(idx, value);
}

std::shared_ptr<SQLiteStatement> SQLiteTable::newStatement(std::string query)
{
    return std::make_shared<SQLiteStatement>(mdb.lock(), query);
}

bool SQLiteTable::execute(SQLiteStatement *stmt)
{
    stmt->execute();
    return true;
}

bool SQLiteTable::hasData(SQLiteStatement *stmt) const
{
    bool hasData = false;
    stmt->executeStep([&hasData]() {
        hasData = true;
        return true;
    });
    return hasData;
}

int SQLiteTable::columnCount(SQLiteStatement *stmt) const
{
    return stmt->columnCount();
}

SQLiteTable::~SQLiteTable() noexcept = default;


size_t SQLiteTable::getLastRowId()
{
    auto db = mdb.lock();
    return sqlite3_last_insert_rowid(db->handle());
}
