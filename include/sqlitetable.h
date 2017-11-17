#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>
#include <memory>
#include <tuple>
#include <sstream>

#include <stdexcept>

#include "sqlitestorage.h"
#include "sqlitefielddef.h"

namespace sqlite {

class SQLiteStorage;
template <typename ...FIELDTYPE>
using TableDef = std::tuple<FieldDef<FIELDTYPE>...>;


template <size_t I, typename ...Ts>
inline std::string buildSqlCreateString(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    return std::string();
};

template <size_t I = 0, typename ...Ts>
inline std::string buildSqlCreateString(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto & field = std::get<I>(def);
    return field.name() + " " + field.sqlType() + field.sqlAttributes()
           + (I == sizeof...(Ts)-1 ? "" : ",") + "\n" +
                                                                                    buildSqlCreateString<I+1, Ts...>(def);
};

class SQLiteTable
{
public:
    class Statement;

private:
    std::weak_ptr<SQLiteStorage> mdb;
    std::string mName;

protected:

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertFieldList(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)def;
        return std::string();
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertFieldList(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto &field = std::get<I>(def);
        return field.name() + (I == sizeof...(Ts)-1 ? "" : ",") + buildSqlInsertFieldList<I+1, Ts...>(def);
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertValuesListPlaceholder(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)def;
        return std::string();
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertValuesListPlaceholder(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto &field = std::get<I>(def);
        std::ostringstream ss;
        ss << "?" << (I+1) << (I == sizeof...(Ts)-1 ? "" : ",") << buildSqlInsertValuesListPlaceholder<I+1, Ts...>(def);
        return ss.str();
    }

    template <typename T>
    void bindValue(Statement *stmt, int idx, T value) {
        static_assert(sizeof(T) == 0, "Generic version of bindValue is undefined");
    }

    template <size_t I, typename ...Ts>
    void bindAllValues (Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
        (void) values; (void)stmt;
    }
    template <size_t I, typename ...Ts>
    void bindAllValues (Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
        bindValue(stmt, I+1, std::get<I>(values));
        bindAllValues<I+1,Ts...>(stmt, values);
    }

    template <typename T>
    void getValue(Statement *stmt, int idx, T &value) {
        static_assert(sizeof(T) == 0, "Generic version of bindValue is undefined");
    }

    template <typename T>
    T getValueR(Statement *stmt, int idx) {
        T t;
        getValue(stmt, idx, t);
        return t;
    }

    template <size_t I, typename ...Ts>
    void getAllValues(Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
    }

    template <size_t I, typename ...Ts>
    void getAllValues(Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        getValue(stmt, I+1, std::get<I>(values));
        getAllValues<I+1, Ts...>(stmt, values);
    }


    template <size_t I, typename ...Ts>
    std::string buildSqlWhereClause(std::tuple<Ts...> &where, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)where;
        return std::string();
    }

    template <size_t I, typename ...Ts>
    std::string buildSqlWhereClause(std::tuple<Ts...> &where, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto &field = std::get<I>(where);
        std::ostringstream ss;
        ss << field.field().name() << "=?" << (I+1) << (I == sizeof...(Ts)-1 ? "" : " AND ") << buildSqlWhereClause<I+1, Ts...>(where);
        return ss.str();
    }

    template <typename ...Ts, typename ...Us, std::size_t... Is>
    auto valuesFromAssignedFields_impl(const std::tuple<Ts...> &fields, std::index_sequence<Is...>)
    {
        return std::make_tuple(std::get<Is>(fields).value()...);
    };

    template <typename ...Ts, typename ...Us>
    auto valuesFromAssignedFields(const std::tuple<Ts...> &fields)
    {
        return valuesFromAssignedFields_impl(fields, std::make_index_sequence<sizeof...(Ts)>{});
    };

public:
    SQLiteTable() {};
    SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    virtual ~SQLiteTable() noexcept;

    bool createFromSQLString (std::string query);

    template <typename ...Ts>
    bool create (std::tuple<Ts...> def) {
        std::ostringstream ss;
        ss << "CREATE TABLE " << mName << " ("
                                       << buildSqlCreateString(def) << ");";
        return createFromSQLString(ss.str());
    }

    std::string name() const { return mName; }

    std::shared_ptr<Statement> newStatement(std::string query);
    bool execute(Statement *stmt);

    bool hasData(Statement *stmt) const;
    int columnCount(Statement *stmt) const;

    template <typename ...Ts, typename ...Us>
    bool insert (std::tuple<Ts...> def, std::tuple<Us...> values) {
        std::ostringstream ss;
        ss << "INSERT INTO " << mName << "("
                                      << buildSqlInsertFieldList<0>(def)
                                      << ") VALUES ("
                                      << buildSqlInsertValuesListPlaceholder<0>(values)
                                      << ");";

        auto stmt = newStatement(ss.str());
        bindAllValues<0>(stmt.get(), values);
        return execute(stmt.get());
    }

    template <typename ...Ts, typename F, std::size_t... Is>
    void getValuesAndCall(Statement *stmt, F resultFeedbackFunc, Ts... value) {

    };

    template <typename ...Ts, typename F, std::size_t... Is>
    void query_impl(std::tuple<Ts...> def, F resultFeedbackFunc, std::index_sequence<Is...> idx) {
        std::ostringstream ss;
        ss << "SELECT " << buildSqlInsertFieldList<0>(def) << " FROM " << mName <<";";
        auto stmt = newStatement(ss.str());

        while (hasData(stmt.get())) {
            auto nColumns = columnCount(stmt.get());
            if (nColumns != sizeof...(Ts))
                throw std::runtime_error("Column count differs from data size");

            resultFeedbackFunc(getValueR<decltype (std::get<Is>(def).rawType())>(stmt.get(), Is)...);
        }
    };

    template <typename ...Ts, typename F>
    void query(std::tuple<Ts...> def, F resultFeedbackFunc) {
        query_impl(def, resultFeedbackFunc, std::make_index_sequence<sizeof...(Ts)>{});
    };

    template <typename ...Ts, typename ...Us, typename F, std::size_t... Is>
    void query_impl(std::tuple<Ts...> def, std::tuple<Us...> where, F resultFeedbackFunc, std::index_sequence<Is...> idx) {
        std::ostringstream ss;
        ss << "SELECT " << buildSqlInsertFieldList<0>(def) << " FROM " << mName <<
           " WHERE " << buildSqlWhereClause<0>(where) << ";";
        auto stmt = newStatement(ss.str());
        auto values = valuesFromAssignedFields(where);
        bindAllValues<0>(stmt.get(), values);

        while (hasData(stmt.get())) {
            auto nColumns = columnCount(stmt.get());
            if (nColumns != sizeof...(Ts))
                throw std::runtime_error("Column count differs from data size");

            resultFeedbackFunc(getValueR<decltype (std::get<Is>(def).rawType())>(stmt.get(), Is)...);
        }
    };

    template <typename ...Ts, typename ...Us, typename F>
    void query(std::tuple<Ts...> def, std::tuple<Us...> where, F resultFeedbackFunc) {
        query_impl(def, where, resultFeedbackFunc, std::make_index_sequence<sizeof...(Ts)>{});
    };

    template <typename ...Ts>
    static SQLiteTable create (std::shared_ptr<SQLiteStorage> db, std::string name, std::tuple<Ts...> def) {
        SQLiteTable table(db, name);

        table.create(def);

        return table;
    }

protected:
    std::shared_ptr<SQLiteStorage> db();
    // helper functions here

};

template <>
void SQLiteTable::bindValue<int> (SQLiteTable::Statement *stmt, int idx, int value);

template <>
void SQLiteTable::getValue<int> (SQLiteTable::Statement *stmt, int idx, int &value);

template <>
void SQLiteTable::bindValue<std::string> (SQLiteTable::Statement *stmt, int idx, std::string value);

template <>
void SQLiteTable::getValue<std::string> (SQLiteTable::Statement *stmt, int idx, std::string &value);

template <>
void SQLiteTable::bindValue<double> (SQLiteTable::Statement *stmt, int idx, double value);


template <typename ...FIELDTYPE>
inline TableDef<FIELDTYPE...> makeTableDef()
{
}

} // ns sqlite

#endif // SQLITETABLE_H
