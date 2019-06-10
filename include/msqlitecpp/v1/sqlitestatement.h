//
// Created by Federico Fuga on 16/12/17.
//

#ifndef SQLITE_SQLITESTATEMENT_H
#define SQLITE_SQLITESTATEMENT_H

#include "msqlitecpp/v1/sqlitefielddef.h"
#include "msqlitecpp/v1/sqlitestatementformatters.h"
#include "msqlitecpp/v1/sqlitestorage.h"

#include <sqlite3.h>

#include <memory>
#include <string>
#include <functional>
#include <cstddef>
#include <iostream>

namespace sqlite {

class EXPORT SQLiteStatement {
    struct Impl;

    std::unique_ptr<Impl> p;

    void init(std::shared_ptr<SQLiteStorage> db);

    void prepare(std::string sql);

    template<size_t I, typename ...Ts>
    void bind_impl(std::tuple<Ts...>, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
    }

    template<size_t I, typename ...Ts>
    void bind_impl(std::tuple<Ts...> t, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        bind(I + 1, std::get<I>(t));  // bind are 1-based, index are 0 based
        bind_impl<I + 1>(t);
    };
public:
    explicit SQLiteStatement();

    explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> dbm, std::string stmt);

    explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> dbm, const char *stmt);

    explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> db, const sqlite::statements::StatementFormatter &stmt);

    SQLiteStatement(SQLiteStatement &&);

    SQLiteStatement &operator=(SQLiteStatement &&);

    ~SQLiteStatement();

    void attach(std::shared_ptr<SQLiteStorage> dbm);

    void attach(std::shared_ptr<SQLiteStorage> dbm, std::string stmt);

    void attach(std::shared_ptr<SQLiteStorage> dbm, const sqlite::statements::StatementFormatter &stmt);

    void prepare(const sqlite::statements::StatementFormatter &stmt);

    void bind(size_t idx, std::string value);

    void bind(size_t idx, unsigned long long value);

    void bind(size_t idx, long long value);

    void bind(size_t idx, unsigned long value);

    void bind(size_t idx, long value);

    void bind(size_t idx, int value);

    void bind(size_t idx, unsigned int value);

    void bind(size_t idx, double value);

    void bind(size_t idx, float value);

    void bind(size_t idx, std::nullptr_t value);

    template<typename ...Ts>
    void bind(std::tuple<Ts...> t)
    {
        bind_impl<0>(t);
    }

    long long getLongValue(int idx);

    unsigned long long getULongValue(int idx);

    int getIntValue(int idx);

    double getDoubleValue(int idx);

    std::string getStringValue(int idx);

    template<typename T>
    T get(int idx)
    {
        static_assert(sizeof(T) == 0, "Generic version of get is undefined");
    }

    bool isNull(int idx);

    FieldType::Type columnType(int idx);

    int columnCount();

    enum class QueryResult {
        Ongoing, Completed, Aborted
    };

    /** @brief Execute a step of a statement, calling a result manipulation function.
     *
     * @args function a functor that treats the results, returns true if ok, false if execution is completed
     * even before the sqlite3 engine has completed the statement.
     * @return true if ok and more data has to come, false if statement execution is completed
     * @throws SqliteException if error occurs
     */
    QueryResult executeStep(std::function<bool()> function);

    QueryResult executeStep();

    bool execute(std::function<bool()> function);

    bool execute();
};

template<>
inline int SQLiteStatement::get<int>(int idx)
{
    return getIntValue(idx);
}

template<>
inline double SQLiteStatement::get<double>(int idx)
{
    return getDoubleValue(idx);
}

template<>
inline long long SQLiteStatement::get<long long>(int idx)
{
    return getLongValue(idx);
}

template<>
inline unsigned long long SQLiteStatement::get<unsigned long long>(int idx)
{
    return getULongValue(idx);
}

template<>
inline std::string SQLiteStatement::get<std::string>(int idx)
{
    return getStringValue(idx);
}

template <typename T>
inline void bind(SQLiteStatement &statement, size_t idx, const T &value) {
    statement.bind(idx, value);
}

} // ns sqlite

#endif //SQLITE_SQLITESTATEMENT_H
