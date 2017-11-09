#include "sqlitetable.h"

#include "sqlitestorage.h"
#include <sqlite3.h>

#include <sstream>

using namespace sqlite;

SQLiteTable::SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : mdb(db), mName(name)
{

}

bool SQLiteTable::create()
{
    auto db = this->db();

    std::ostringstream ss;
    ss << "CREATE TABLE " << name() << " (" << getCreateDefinition() << ");";
    auto query = ss.str();

    sqlite3_stmt *s;
    if (sqlite3_prepare(db->handle(), query.c_str(), query.size(), &s, nullptr) != SQLITE_OK)
        throw SQLiteException(db->handle());

    if (sqlite3_step(s) != SQLITE_DONE) {
        throw SQLiteException(db->handle());
    }

    return true;
}

std::shared_ptr<SQLiteStorage> SQLiteTable::db()
{
    auto db = mdb.lock();

    if (db == nullptr)
        throw std::logic_error("Operation on an orphaned Table!");
    return db;
}

SQLiteTable::~SQLiteTable() noexcept = default;

