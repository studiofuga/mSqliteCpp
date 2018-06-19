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
        if (!constraint.empty())
            constraint += ", ";
        constraint += statements::details::toString(tableConstraint);
    }

    std::string statementString()
    {
        createStatement();
        return statement.string();
    }
};

template<typename ...FIELDS>
class CreateIndexStatement {
    bool mUnique = false;
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    std::string mIndexName;
    std::string mTableName;
    mutable statements::CreateIndex statement;

    template<std::size_t ...I>
    void buildStatementImpl(std::index_sequence<I...>) {
        if (mUnique)
            statement = statements::CreateIndex(statements::CreateIndex::Unique, mIndexName, mTableName, std::get<I>(fields)...);
        else
            statement = statements::CreateIndex(mIndexName, mTableName, std::get<I>(fields)...);
    }

    void buildStatement() {
        buildStatementImpl(std::index_sequence_for<FIELDS...>{});
    }

public:
    CreateIndexStatement() = default;

    explicit CreateIndexStatement(std::string indexname, FIELDS... f)
    : mIndexName(indexname), fields(std::make_tuple(f...))
    {
    }

    CreateIndexStatement(std::shared_ptr<SQLiteStorage> dbm, std::string indexname, std::string table, FIELDS... f)
    : mIndexName(indexname), fields(std::make_tuple(f...))
    {
        attach(dbm, table);
    }

    CreateIndexStatement &unique() { mUnique = true;  return *this; }
    CreateIndexStatement &attach(std::shared_ptr<SQLiteStorage> dbm, std::string table)
    {
        db = dbm;
        mTableName = std::move(table);
        return *this;
    }

    void execute()
    {
        buildStatement();
        auto pstatement = std::make_shared<SQLiteStatement>(db, statement);
        pstatement->execute();
    }

    std::string statementString()
    {
        buildStatement();
        return statement.string();
    }
};

template <typename ...FIELDS>
inline CreateTableStatement<FIELDS...> makeCreateTableStatement2(std::shared_ptr<SQLiteStorage> db,
                                                                std::string table, FIELDS ...fields) {
    return CreateTableStatement<FIELDS...>(db, table, fields...);
}
template <typename ...FIELDS>
inline CreateTableStatement<FIELDS...> makeCreateTableStatement(FIELDS ...fields) {
    return CreateTableStatement<FIELDS...>(fields...);
}

template <typename ...FIELDS>
inline CreateIndexStatement<FIELDS...> makeCreateIndexStatement(std::shared_ptr<SQLiteStorage> db,
                                                                std::string indexname,
                                                                std::string table,
                                                                FIELDS ...fields) {
    return CreateIndexStatement<FIELDS...>(db, indexname, table, fields...);
}

template <typename ...FIELDS>
inline CreateIndexStatement<FIELDS...> makeCreateUniqueIndexStatement(std::shared_ptr<SQLiteStorage> db,
                                                                std::string indexname,
                                                                std::string table,
                                                                FIELDS ...fields) {
    return makeCreateIndexStatement(db, indexname, table, fields...).unique();
}

}

#endif //SQLITE_CREATESTATEMENT_H
