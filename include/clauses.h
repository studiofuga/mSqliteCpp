//
// Created by Federico Fuga on 06/04/18.
//

#ifndef SQLITE_CLAUSES_H
#define SQLITE_CLAUSES_H

#include "sqlitestatement.h"
#include "operators.h"

#include <string>

namespace sqlite {

template<typename ...Ts>
class Where {
    SQLiteStatement *statement;
    std::string condition;
    int bindOffset = 0;

    template<size_t I, typename ...Vs>
    void bindImpl(std::tuple<Vs...>, typename std::enable_if<I == sizeof...(Vs)>::type * = 0)
    {
    }

    template<size_t I, typename ...Vs>
    void bindImpl(std::tuple<Vs...> t, typename std::enable_if<I < sizeof...(Vs)>::type * = 0)
    {
        statement->bind(I + 1 + bindOffset, std::get<I>(t));  // bind are 1-based, index are 0 based
        bindImpl<I + 1>(t);
    };

public:
    Where() = default;

    explicit Where(SQLiteStatement *refStatement, std::string cond)
            : statement(refStatement), condition(std::move(cond))
    {
    }

    void attach(SQLiteStatement *refStatement, std::string cond)
    {
        statement = refStatement;
        condition = std::move(cond);
    }

    std::string toText()
    {
        return condition;
    }

    void setBindOffset(int n)
    {
        bindOffset = n;
    }

    template<typename ...Vs>
    void bind(Vs... t)
    {
        bindImpl<0>(std::make_tuple(t...));
    }

    template<typename V>
    void bindN(size_t index, V v) {
        statement->bind(index, v);
    }
};


template<typename OP, typename ...Ts>
class WhereExt {
    SQLiteStatement *statement;
    OP condition;
    int bindOffset = 0;

    template<size_t I, typename ...Vs>
    void bindImpl(std::tuple<Vs...>, typename std::enable_if<I == sizeof...(Vs)>::type * = 0)
    {
    }

    template<size_t I, typename ...Vs>
    void bindImpl(std::tuple<Vs...> t, typename std::enable_if<I < sizeof...(Vs)>::type * = 0)
    {
        statement->bind(I + 1 + bindOffset, std::get<I>(t));  // bind are 1-based, index are 0 based
        bindImpl<I + 1>(t);
    };

    template <typename Z>
    std::string formatCondition(Z s) {
        return s(bindOffset);
    }

    std::string formatCondition(std::string x) {
        return x;
    }

public:
    WhereExt() = default;

    explicit WhereExt(SQLiteStatement *refStatement, OP cond)
            : statement(refStatement), condition(std::move(cond))
    {
    }

    void attach(SQLiteStatement *refStatement, OP cond)
    {
        statement = refStatement;
        condition = std::move(cond);
    }

    std::string toText()
    {
        return formatCondition(condition);
    }

    void setBindOffset(int n)
    {
        bindOffset = n;
    }

    template<typename ...Vs>
    void bind(Vs... t)
    {
        bindImpl<0>(std::make_tuple(t...));
    }
};

template<typename OP>
class WhereOpt {
    SQLiteStatement *statement;
    OP condition;
    int bindOffset = 0;

public:
    WhereOpt() = default;
    WhereOpt(OP cond)
            : condition(std::move(cond)) {
    }

    void setBindOffset(int n)
    {
        bindOffset = n;
    }

    template <typename ...Vs>
    std::string format(Vs... v) {
        auto condformat = condition.format(bindOffset, v...);
        if (condformat.empty()) {
            condformat = "1";
        }
        return condformat;
    }
};


template <typename OP, typename ...Ts>
WhereExt<OP, Ts...> makeWhere(SQLiteStatement *refStatement, OP op, Ts... t) {
    WhereExt<OP,Ts...> w(refStatement, op);
    return w;
};

template <typename OP>
WhereOpt<OP> makeWhereOpt(OP op) {
    return WhereOpt<OP>(op);
};

template <typename ...Ts>
Where<std::string, Ts...> makeWhere (Ts...t)
{
    return Where<std::string, Ts...>(t...);
};

}

#endif //SQLITE_CLAUSES_H
