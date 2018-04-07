//
// Created by Federico Fuga on 06/04/18.
//

#ifndef SQLITE_CLAUSES_H
#define SQLITE_CLAUSES_H

namespace sqlite {

template <typename ...Ts>
class Where {
    SQLiteStatement *statement;
    std::string condition;

    template <size_t I, typename ...Vs>
    void bindImpl(std::tuple<Vs...>, typename std::enable_if<I == sizeof...(Vs)>::type * = 0) {
    }

    template <size_t I, typename ...Vs>
    void bindImpl(std::tuple<Vs...> t, typename std::enable_if<I < sizeof...(Vs)>::type * = 0) {
        statement->bind(I+1, std::get<I>(t));  // bind are 1-based, index are 0 based
        bindImpl<I+1>(t);
    };

public:
    explicit Where (SQLiteStatement *refStatement, std::string cond)
    : statement(refStatement), condition (std::move(cond)) {
    }

    std::string toText() {
        return condition;
    }

    template <typename ...Vs>
    void bind (Vs... t) {
        bindImpl<0>(std::make_tuple(t...));
    }
};

}

#endif //SQLITE_CLAUSES_H
