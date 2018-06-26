//
// Created by Federico Fuga on 07/04/18.
//

#ifndef SQLITE_UPDATESTATEMENT_H
#define SQLITE_UPDATESTATEMENT_H

#include <tuple>
#include <type_traits>
#include <utility>

#include "sqlitestorage.h"
#include "sqlitestatement.h"

namespace sqlite {

template<typename ...FIELDS>
class UpdateStatement {
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    std::string tablename;
    bool statementDirty = true;
    std::shared_ptr<SQLiteStatement> statement = std::make_shared<SQLiteStatement>();
    statements::Update sql;

    template <typename T>
    bool bindValue(int idx, const T& t) {
        sqlite::bind(*statement, idx+1, t);
        return true;
    }

    template <typename T>
    bool bindValue(int idx, const boost::optional<T> & t) {
        if (t) {
            sqlite::bind(*statement, idx+1, t.value());
            return true;
        }
        return false;
    }

    bool bindValue(int idx, std::nullptr_t t) {
        sqlite::bind(*statement, idx+1, nullptr);
        return true;
    }

    template<int N = 0, typename ...T>
    typename std::enable_if<N == sizeof...(T), void>::type updateImpl(std::tuple<T...>, size_t idx = 0)
    {
    };

    template<int N = 0, typename ...T>
    typename std::enable_if<N < sizeof...(T), void>::type updateImpl(std::tuple<T...> values, size_t idx = 0)
    {
        bindValue(idx, std::get<N>(values));
        ++idx;
        updateImpl<N + 1>(values, idx);
    };

public:
    UpdateStatement()
    {}

    explicit UpdateStatement(FIELDS... f)
    { fields = std::make_tuple(f...); }

    void attach(std::shared_ptr<SQLiteStorage> dbm, std::string table)
    {
        db = dbm;
        tablename = std::move(table);
        statement->attach(db);
        prepare();
    }

    void prepare() {
        sql = statements::Update(tablename, fields);
        statement->prepare(sql);
        statementDirty = false;
    }

    template<typename W>
    void where(W &w)
    {
        sql.where(w.toText());
    }

    template<typename ...T>
    void update(T... values)
    {
        if (statementDirty)
            prepare();

        updateImpl<0>(std::make_tuple(values...));
        statement->execute();
    }

    template<typename ...T>
    void unpreparedUpdate(T... values)
    {
        sql = statements::Update(tablename, fields, std::make_tuple(values...));
        statement->prepare(sql);

        updateImpl<0>(std::make_tuple(values...));
        statement->execute();
        statementDirty = true;
    }

    SQLiteStatement *getStatement()
    {
        return statement.get();
    }
};

}

#endif //SQLITE_UPDATESTATEMENT_H
