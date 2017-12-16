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
        struct Impl;

        std::unique_ptr<Impl> p;
    public:
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> db, std::string sql);
        ~SQLiteStatement();

        void bind(int idx, std::string value);

        bool execute(std::function<bool()>);
    };

} // ns sqlite

#endif //SQLITE_SQLITESTATEMENT_H
