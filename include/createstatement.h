/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 08/05/18
 */

#ifndef SQLITE_CREATESTATEMENT_H
#define SQLITE_CREATESTATEMENT_H

#include "sqlitestorage.h"
#include "sqlitestatement.h"

#include <type_traits>
#include <functional>
#include <tuple>
#include <utility>
#include <iostream>

namespace sqlite {

template<typename ...FIELDS>
class CreateTableStatement {
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    mutable statements::CreateTable statement;
    std::string tablename;
    std::string constraint;

    void createStatement() {
        statement = statements::CreateTable(tablename, fields);
        if (!constraint.empty())
            statement.setConstraint(constraint);
    }
    bool attached() const {
        return !tablename.empty();
    }
public:
    CreateTableStatement() = default;

    explicit CreateTableStatement(FIELDS... f)
    {
        fields = std::make_tuple(f...);
    }

    CreateTableStatement(std::shared_ptr<SQLiteStorage> dbm, std::string table, FIELDS... f)
    {
        fields = std::make_tuple(f...);
        attach(dbm, table);
    }

    void attach(std::shared_ptr<SQLiteStorage> dbm, std::string table)
    {
        db = dbm;
        tablename = std::move(table);
    }

    void execute()
    {
        createStatement();
        auto pstatement = std::make_shared<SQLiteStatement>(db, statement);
        pstatement->execute();
    }

    template<typename T>
    void setTableConstraint(T tableConstraint)
    {
        statement.setConstraint(tableConstraint);
        constraint = statements::details::toString(tableConstraint);
    }

    std::string statementString()
    {
        createStatement();
        return statement.string();
    }
};
}

#endif //SQLITE_CREATESTATEMENT_H
