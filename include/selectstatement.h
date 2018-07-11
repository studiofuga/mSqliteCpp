//
// Created by Federico Fuga on 14/03/18.
//

#ifndef SQLITE_SELECTSTATEMENT_H
#define SQLITE_SELECTSTATEMENT_H

#include <boost/optional.hpp>

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
    auto getValues()
    {
        return statement.get<decltype(std::get<I>(fields).rawType())>(I);
    };

    template<std::size_t I>
    auto getValuesOpt()
    {
        if (statement.isNull(I))
            return boost::optional<decltype(std::get<I>(fields).rawType())>();
        return boost::make_optional(statement.get<decltype(std::get<I>(fields).rawType())>(I));
    };

    template<typename F, std::size_t ...Is>
    bool execImpl(F func, std::index_sequence<Is...> const &i)
    {
        return func(getValues<Is>()...);
    };

    template<typename F, std::size_t ...Is>
    bool execImplOpt(F func, std::index_sequence<Is...> const &i)
    {
        return func(getValuesOpt<Is>()...);
    };

    template <typename T>
    bool bindValue(int idx, const T& t) {
        sqlite::bind(statement, idx+1, t);
        return true;
    }

    template <typename T>
    bool bindValue(int idx, const boost::optional<T> & t) {
        if (t) {
            sqlite::bind(statement, idx+1, t.value());
            return true;
        }
        return false;
    }

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
    explicit SelectStatement(Fs... f)
            : fields(std::make_tuple(f...))
    {

    }

    void attach(std::shared_ptr<SQLiteStorage> db, std::string name)
    {
        sql = statements::Select(name, fields);
        this->db = db;
        this->name = std::move(name);
        statement.attach(db);
    }

    template<typename ...FLDS>
    void groupBy(FLDS... fields)
    {
        sql.groupBy(fields...);
    }

    void prepare()
    {
        statement.prepare(sql);
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

    template<typename F>
    void execOpt(F func)
    {
        statement.execute([this, func]() {
            return execImplOpt(func, std::make_index_sequence<sizeof...(Fs)>{});
        });
    }

    template <typename ...Vs>
    void bind(Vs... values) {
        bindImpl<0>(std::make_tuple(values...), 0);
    }
};

template<typename ...Fs>
inline SelectStatement<Fs...> makeSelectStatement (Fs... fs)
{
    return SelectStatement<Fs...>(fs...);
}


}

#endif //SQLITE_SELECTSTATEMENT_H
