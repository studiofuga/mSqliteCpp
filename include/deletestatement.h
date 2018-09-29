//
// Created by Federico Fuga on 07/04/18.
//

#ifndef SQLITE_DELETESTATEMENT_H
#define SQLITE_DELETESTATEMENT_H

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>
#include "sqlitestorage.h"
#include "sqlitestatement.h"

namespace sqlite {

class DeleteStatement {
    std::shared_ptr<SQLiteStorage> db;
    std::string name;
    SQLiteStatement statement;

    statements::Delete sql;

    template <typename T>
    bool bindValue(int idx, const T& t) {
        sqlite::bind(statement, idx+1, t);
        return true;
    }

#if defined(WITH_BOOST)
    template <typename T>
    bool bindValue(int idx, const boost::optional<T> & t) {
        if (t) {
            sqlite::bind(statement, idx+1, t.value());
            return true;
        }
        return false;
    }
#endif

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
    DeleteStatement() = default;

    void attach(std::shared_ptr<SQLiteStorage> db, std::string name)
    {
        sql = statements::Delete(name);
        this->db = db;
        this->name = std::move(name);
    }

    std::string sqlString() const {
        return sql.string();
    }

    void prepare()
    {
        statement.attach(db, sql);
    }

    template<typename W>
    void where(W w)
    {
        sql.where(w.toText());
    }

    void where(std::string s)
    {
        sql.where(s);
    }

    void where()
    {
        sql.where("");
    }

    void exec()
    {
        statement.execute();
    }

    SQLiteStatement *getStatement()
    {
        return &statement;
    }

    template <typename ...Vs>
    void bind(Vs... values) {
        bindImpl<0>(std::make_tuple(values...), 0);
    }

};

}

#endif //SQLITE_DELETESTATEMENT_H
