//
// Created by Federico Fuga on 16/12/17.
//

#ifndef SQLITE_SQLITESTATEMENT_H
#define SQLITE_SQLITESTATEMENT_H

#include <sqlitestorage.h>

#include <memory>
#include <string>

#include <sqlite3.h>

namespace sqlite {

    class SQLiteStatement {
        SQLiteStorage *mDb;
        sqlite3_stmt *stmt = nullptr;
    public:
        explicit SQLiteStatement(SQLiteStorage *db,
                                 std::string sql) :
                mDb(db)
        {
            auto r = sqlite3_prepare_v2(mDb->handle(), sql.c_str(), -1, &stmt, nullptr);
            if (r != SQLITE_OK)
                throw SQLiteException(mDb->handle());
        }

        ~SQLiteStatement() {
            if (stmt != nullptr)
                sqlite3_finalize(stmt);
        }

        sqlite3_stmt *handle() const { return stmt; }

        SQLiteStorage *db() const { return mDb; }
    };

} // ns sqlite

#endif //SQLITE_SQLITESTATEMENT_H
