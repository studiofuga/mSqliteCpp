//
// Created by Federico Fuga on 06/04/18.
//

#ifndef SQLITE_CLAUSES_H
#define SQLITE_CLAUSES_H

#include "sqlitestatement.h"

#include <string>

namespace sqlite {

template<typename OP, typename ...Ts>
class Where {
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
        std:: cout << "BIND: " << I+1+bindOffset << " = " << std::get<I>(t) << "\n";
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
    Where() = default;

    explicit Where(SQLiteStatement *refStatement, OP cond)
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

template <typename OP, typename ...Ts>
Where<OP, Ts...> makeWhere(SQLiteStatement *refStatement, OP op, Ts... t) {
    Where<OP,Ts...> w(refStatement, op);
    return w;
};

template <typename ...Ts>
Where<std::string, Ts...> makeWhere (Ts...t)
{
    return Where<std::string, Ts...>(t...);
};

}

#endif //SQLITE_CLAUSES_H
