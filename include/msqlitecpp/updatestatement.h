//
// Created by Federico Fuga on 07/04/18.
//

#ifndef SQLITE_UPDATESTATEMENT_H
#define SQLITE_UPDATESTATEMENT_H

#include "msqlitecpp/sqlitestorage.h"
#include "msqlitecpp/sqlitestatement.h"
#include "msqlitecpp/clauses.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace sqlite {

template<typename ...FIELDS>
class UpdateStatement {
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    std::string tablename;
    bool statementDirty = true;
    std::shared_ptr<SQLiteStatement> statement = std::make_shared<SQLiteStatement>();
    statements::Update sql;
    std::string whereText;

    template <typename T>
    bool bindValue(int idx, const T& t) {
        sqlite::bind(*statement, idx+1, t);
        return true;
    }

#if defined(WITH_BOOST)
    template <typename T>
    bool bindValue(int idx, const boost::optional<T> & t) {
        if (t) {
            sqlite::bind(*statement, idx+1, t.value());
            return true;
        }
        return false;
    }
#endif

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

    template <size_t I = 0, typename ...Vs, typename std::enable_if<I == sizeof...(Vs), int>::type = 0>
    void bindImpl(std::tuple<Vs...>, size_t = 0)
    {
    }

    template <size_t I = 0, typename ...Vs, typename std::enable_if<I < sizeof...(Vs), int>::type = 0>
    void bindImpl(std::tuple<Vs...> vs, size_t offs = 0) {
        bindValue(I+offs, std::get<I>(vs));
        bindImpl<I+1>(vs,offs);
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
        sql.where(whereText);
        statement->prepare(sql);
        statementDirty = false;
    }

    template<typename ...T>
    void prepare(T... values) {
        sql = statements::Update(tablename, fields, std::make_tuple(values...));
        sql.where(whereText);
        statement->prepare(sql);
        statementDirty = false;
    }

    std::string sqlString() const { return sql.string(); }

    void where()
    {
        whereText.clear();
        sql.where(whereText);
    }

    template<typename W>
    void where(W &w)
    {
        w.setBindOffset(sizeof...(FIELDS));
        whereText = w.toText();
        sql.where(whereText);
    }

    template<typename OP, typename ...F>
    void where(WhereOpt<OP> w, F... f)
    {
        w.setBindOffset(sizeof...(FIELDS));
        whereText = w.format(f...);
        sql.where(whereText);
    }

    template <typename ...Vs>
    void bind(Vs... values) {
        bindImpl<0>(std::make_tuple(values...), sizeof...(FIELDS));
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
        updateImpl<0>(std::make_tuple(values...));
        statement->execute();
        statementDirty = true;
    }

    SQLiteStatement *getStatement()
    {
        return statement.get();
    }
};

template<typename ...FIELDS>
UpdateStatement<FIELDS...> makeUpdateStatement(FIELDS... fields) {
    return UpdateStatement<FIELDS...>(fields...);
}

template<typename ...FIELDS>
UpdateStatement<FIELDS...> makeUpdateStatement2(std::shared_ptr<SQLiteStorage> db, std::string table, FIELDS... fields) {
    auto u = UpdateStatement<FIELDS...>(fields...);
    u.attach(db, table);
    return u;
}

}

#endif //SQLITE_UPDATESTATEMENT_H
