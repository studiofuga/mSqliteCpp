//
// Created by Federico Fuga on 14/03/18.
//

#ifndef SQLITE_SELECTSTATEMENT_H
#define SQLITE_SELECTSTATEMENT_H

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>

namespace sqlite {

template<typename ...Fs>
class SelectStatement {
private:
    std::shared_ptr<SQLiteStorage> db;
    std::string name;
    std::tuple<Fs...> fields;
    SQLiteStatement statement;
    statements::Select sql;

    /// Impl

    /**
     * Get a single return value from the statement. The returned type is the Field::RawType()
     * @tparam I the index of the value
     * @return The returned value as from SQLiteStatement::get<T>(I);
     */
    template<std::size_t I>
    auto getValue()
    {
        return statement.get<decltype(std::get<I>(fields).rawType())>(I);
    };

    template<typename F, std::size_t ...Is>
    bool execImpl(F func, std::index_sequence<Is...> const &i)
    {
        return func(getValue<Is>()...);
    };
public:
    explicit SelectStatement(Fs... f)
            : fields(std::make_tuple(f...))
    {

    }

    void attach(std::shared_ptr<SQLiteStorage> db, std::string name)
    {
        sql = statements::Select(name, fields);
        this->db = db;
        this->name = std::move(name);
    }

    template<typename ...FLDS>
    void groupBy(FLDS... fields)
    {
        sql.groupBy(fields...);
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

    SQLiteStatement *getStatement()
    {
        return &statement;
    }

    std::string statementString() const
    {
        return sql.string();
    }

    template<typename F>
    void exec(F func)
    {
        statement.execute([this, func]() {
            return execImpl(func, std::make_index_sequence<sizeof...(Fs)>{});
        });
    }
};

template<typename ...Fs>
inline SelectStatement<Fs...> makeSelectStatement (Fs... fs)
{
    return SelectStatement<Fs...>(fs...);
}


}

#endif //SQLITE_SELECTSTATEMENT_H
