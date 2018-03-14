//
// Created by Federico Fuga on 14/03/18.
//

#ifndef SQLITE_SELECTSTATEMENT_H
#define SQLITE_SELECTSTATEMENT_H

#include <functional>
#include <tuple>

namespace sqlite {

template <typename Tag, typename ...T>
struct SqlTuple {
    using TupleType = std::tuple<T...>;
    std::tuple<T...> asTuple;

    explicit SqlTuple(std::tuple<T...> t)
    : asTuple(t) {}
    SqlTuple(T... t)
    : asTuple (std::make_tuple(t...)) {}
};

struct SelectTag {};
template <typename ...T>
using SelectTuple = SqlTuple<SelectTag, T...>;

struct WhereTag {};
template <typename ...T>
using WhereTuple = SqlTuple<WhereTag, T...>;

struct BindTag {};
template <typename ...T>
using BindTuple = SqlTuple<BindTag, T...>;

template <typename S, typename W>
class SelectStatement {
    S selectFields;
    std::shared_ptr<SQLiteStorage> db;
    std::shared_ptr<SQLiteStatement> statement;
    std::string tablename;

    template <int N=0, typename ...T>
    typename std::enable_if<N < sizeof...(T), void>::type bindImpl(std::tuple<T...> values) {
        statement->bind(N+1, std::get<N>(values));
    };

    template <int N = 0, typename ...TA, std::enable_if<N < sizeof...(TA)> >
    auto getImpl(const std::tuple<TA...> &tpl) {
        using T = decltype(std::get<N>(tpl)->rawType());
        return std::tuple_cat(std::make_tuple(statement->get<T>(N)), getImpl<N+1>(tpl));
    };

public:
    SelectStatement(S s)
            : selectFields(s) {
    }

    void attach (std::shared_ptr<SQLiteStorage> dbm, std::string table) {
        db = dbm;
        tablename = std::move(table);
        statement = std::make_shared<SQLiteStatement>(db, statements::Select(tablename, selectFields.asTuple));
    }

    template <typename ...BT>
    void bind(BT... b) {
        bindImpl<0>(std::make_tuple(b...));
    }

    void exec () {
        statement->execute([this](){
            auto r = getImpl<0>(selectFields.asTuple);
            return true;
        });
    }
};

template <typename ...T>
inline SelectTuple<T...> Select(T... t) {
    return SelectTuple<T...>(t...);
}

template <typename ...T>
inline WhereTuple<T...> Where(T... t) {
    return WhereTuple<T...>(t...);
}

template <typename ...T>
inline BindTuple<T...> Bind(T... t) {
    return BindTuple<T...>(t...);
}

template <typename ...T, typename ...U>
inline SelectStatement<SelectTuple<T...>,WhereTuple<U...>> makeSelectStatement(SelectTuple<T...> s, WhereTuple<U...> w) {
    return SelectStatement<decltype(s), decltype(w)>(s);
}

}

#endif //SQLITE_SELECTSTATEMENT_H
