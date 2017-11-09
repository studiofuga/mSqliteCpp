#include "sqlitetable.h"

#include "sqlitestorage.h"
#include <sqlite3.h>

#include <sstream>
#include <iostream>

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
    std::cout <<  "Executing: " << query << "\n";

    auto db = this->db();

    sqlite3_stmt *s;
    if (sqlite3_prepare(db->handle(), query.c_str(), query.size(), &s, nullptr) != SQLITE_OK)
        throw SQLiteException(db->handle());

    if (sqlite3_step(s) != SQLITE_DONE) {
        throw SQLiteException(db->handle());
    }

    return true;
}

SQLiteTable::~SQLiteTable() noexcept = default;

