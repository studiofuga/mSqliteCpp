/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_STATEMENT_H
#define MSQLITECPP_STATEMENT_H

#include "msqlitecpp/v2/msqlitecpp.h"
#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"

#include "msqlitecpp/utils/spimpl.h"

#include <functional>

namespace msqlitecpp {
namespace v2 {

/**
 * @brief An SQL Statement
 *
 * The class is moveable but not copyable because of the raw pointer to the sqlite3_stmt structure.
 */
class EXPORT_MSQLITEV2 Statement {
private:
    struct Impl;

    spimpl::impl_ptr<Impl> p;

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
    explicit Statement(char const *sql);

    explicit Statement(Storage &);

    explicit Statement(Storage &, char const *sql);

    Statement(Statement const &) = delete;
    Statement(Statement &&) = default;

    Statement &operator =(Statement const &) = delete;
    Statement &operator =(Statement &&) = default;

    bool execute(Storage &handle);

    virtual ~Statement();

    void set(char const *sql);

    void set(std::string const &sql);

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

    ColumnTypes::Type columnType(int idx);

    int columnCount();


    std::string toString() const;
};


template<typename T>
inline void bind(Statement &statement, size_t idx, const T &value)
{
    statement.bind(idx, value);
}

}
}

#endif //MSQLITECPP_STATEMENT_H
