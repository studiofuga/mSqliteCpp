//
// Created by Federico Fuga on 16/12/17.
//

#ifndef SQLITE_SQLITESTATEMENT_H
#define SQLITE_SQLITESTATEMENT_H

#include <sqlitestorage.h>

#include <memory>
#include <string>

#include <sqlite3.h>
#include "sqlitefielddef.h"
#include "sqlitestatementformatters.h"

namespace sqlite {

    class SQLiteStatement {
        struct Impl;

        std::unique_ptr<Impl> p;

        void init(std::shared_ptr<SQLiteStorage> db);
        void prepare (std::string sql);
    public:
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> dbm, std::string stmt);
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> dbm, const char *stmt);
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> db, const sqlite::statements::StatementFormatter &stmt);

        ~SQLiteStatement();

        void bind(int idx, std::string value);
        void bind(int idx, int value);
        void bind(int idx, double value);

        int getIntValue(int idx);
        double getDoubleValue(int idx);
        std::string getStringValue(int idx);

        FieldType::Type columnType(int idx);
        int columnCount();

        /** @brief Execute a step of a statement, calling a result manipulation function.
         *
         * @args function a functor that treats the results, returns true if ok, false if execution is completed
         * even before the sqlite3 engine has completed the statement.
         * @return true if ok and more data has to come, false if statement execution is completed
         * @throws SqliteException if error occurs
         */
        bool execute(std::function<bool()> function);
        bool execute();
    };

} // ns sqlite

#endif //SQLITE_SQLITESTATEMENT_H
