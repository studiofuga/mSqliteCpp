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

namespace sqlite {

    class SQLiteStatement {
        struct Impl;

        std::unique_ptr<Impl> p;
    public:
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> db, std::string sql);
        ~SQLiteStatement();

        void bind(int idx, std::string value);
        void bind(int idx, int value);
        void bind(int idx, double value);

        int getIntValue(int idx);
        double getDoubleValue(int idx);
        std::string getStringValue(int idx);

        FieldType::Type columnType(int idx);
        int columnCount();

        bool execute(std::function<bool()>);
        bool execute();
    };

} // ns sqlite

#endif //SQLITE_SQLITESTATEMENT_H
