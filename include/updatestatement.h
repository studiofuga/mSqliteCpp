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
    SQLiteStatement statement;
    statements::Update sql;

    template <int N=0, typename ...T>
    typename std::enable_if<N == sizeof...(T), void>::type updateImpl(std::tuple<T...>) {
    };

    template <int N=0, typename ...T>
    typename std::enable_if<N < sizeof...(T), void>::type updateImpl(std::tuple<T...> values) {
        statement.bind(N+1, std::get<N>(values));
        updateImpl<N + 1>(values);
    };

public:
    UpdateStatement() {}
    explicit UpdateStatement(FIELDS... f) { fields = std::make_tuple(f...); }

    void attach (std::shared_ptr<SQLiteStorage> dbm, std::string table) {
        db = dbm;
        tablename = std::move(table);
        sql = statements::Update(tablename, fields);
    }

    void prepare() {
        statement.attach(db, sql);
    }

    template <typename W>
    void where (W &w) {
        w.setBindOffset(sizeof...(FIELDS));
        sql.where(w.toText());
    }

    template <typename ...T>
    void update (T... values) {
        updateImpl<0>(std::make_tuple(values...));
        statement.execute();
    }

    SQLiteStatement *getStatement() {
        return &statement;
    }
};

}

#endif //SQLITE_UPDATESTATEMENT_H
