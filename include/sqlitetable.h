#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include "msqlitecpp.h"

#include <string>
#include <memory>
#include <tuple>
#include <sstream>

#include <stdexcept>

#include "sqlitestorage.h"
#include "sqlitefielddef.h"
#include <iostream>

namespace sqlite {

class SQLiteStorage;
template <typename ...FIELDTYPE>
using TableDef = std::tuple<FieldDef<FIELDTYPE>...>;

/// @brief A Database Table object
class EXPORT SQLiteTable
{
public:
    /// @brief an opaque type to handle SQLite statements.
    class Statement;

private:
    std::weak_ptr<SQLiteStorage> mdb;
    std::string mName;

protected:
    template <typename T>
    std::string sqlFieldCreateSpec(const std::vector<T> &fields) const {
        std::ostringstream ss;
        auto i = std::begin(fields);
        auto e = std::end(fields);
        while (i != e) {
            ss << sqlFieldCreateSpec(*i);
            ++i;
            if (i != e)
                ss << ",\n";
        }
        return ss.str();
    }

    template <typename T>
    std::string sqlFieldCreateSpec(T field) const {
        return field.name() + " " + field.sqlType() + field.sqlAttributes();
    }

    template <size_t I, typename ...Ts>
    inline std::string buildSqlCreateString(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)def;
        return std::string();
    };

    template <size_t I = 0, typename ...Ts>
    inline std::string buildSqlCreateString(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto & field = std::get<I>(def);
        return sqlFieldCreateSpec(field)
               + (I == sizeof...(Ts)-1 ? "" : ",") + "\n" +
               buildSqlCreateString<I+1, Ts...>(def);
    };

    // ** Create a "INSERT" SQL statement from a tuple of fields definition

    template <size_t I, typename ...Ts>
    std::string buildSqlInsertFieldList(std::tuple<Ts...> &def, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)def;
        return std::string();
    }

    /// @brief builds a Field list part of a SQL Insert statement
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

    /// @brief builds a Values part of a SQL Insert statement
    template <size_t I, typename ...Ts>
    std::string buildSqlInsertValuesListPlaceholder(std::tuple<Ts...> &def, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        std::ostringstream ss;
        ss << "?" << (I+1) << (I == sizeof...(Ts)-1 ? "" : ",") << buildSqlInsertValuesListPlaceholder<I+1, Ts...>(def);
        return ss.str();
    }

    template <size_t I,typename ...Ts>
    std::string buildSqlUpdateColumnStatement(std::tuple<Ts...> nm, typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
        (void)nm;
        return std::string();
    }

    template <size_t I,typename ...Ts>
    std::string buildSqlUpdateColumnStatement(std::tuple<Ts...> nm, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        std::ostringstream ss;
        ss << std::get<I>(nm).name() << "=?" << (I+1) << (I+1 == sizeof...(Ts) ? "" : ",") << buildSqlUpdateColumnStatement<I+1>(nm);
        return ss.str();
    }

    /// @brief builds the fields part of an update statement  (UPDATE tablename SET field=? ....)
    template <typename ...Ts>
    std::string buildSqlUpdateColumnsStatement_impl(std::tuple<Ts...> def) {
        std::ostringstream ss;
        ss << buildSqlUpdateColumnStatement<0>(def);
        return ss.str();
    }

    template <typename ...Ts>
    std::string buildSqlUpdateColumnsStatement(std::tuple<Ts...> def) {
        return buildSqlUpdateColumnsStatement_impl(def);
    }

    /// @brief Binds a value to a placeholder in an SQL Statement
    template <typename T>
    void bindValue(Statement *stmt, int idx, T value) {
        (void)stmt; (void)idx; (void)value;
        static_assert(sizeof(T) == 0, "Generic version of bindValue is undefined");
    }

    /// @brief Binds all the values in an SQL statement (End recursion version)
    template <size_t I, size_t Start = 0, typename ...Ts>
    void bindAllValues (Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
        (void) values; (void)stmt;
    }

    /// @brief Binds all the values in an SQL statement
    template <size_t I, size_t Start = 0, typename ...Ts>
    void bindAllValues (Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
        bindValue(stmt, I+Start+1, std::get<I>(values));
        bindAllValues<I+1,Start, Ts...>(stmt, values);
    }

    template <typename T>
    void getValue(Statement *stmt, int idx, T &value) {
        (void)stmt; (void)idx; (void)value;
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
        (void)stmt; (void)values;
    }

    template <size_t I, typename ...Ts>
    void getAllValues(Statement *stmt, std::tuple<Ts...> &values, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        getValue(stmt, I+1, std::get<I>(values));
        getAllValues<I+1, Ts...>(stmt, values);
    }


    template <size_t I, size_t Start = 0, typename ...Ts>
    std::string buildSqlWhereClause(std::tuple<Ts...> &where, typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
    {
        (void)where;
        return std::string();
    }

    template <size_t I, size_t Start = 0, typename ...Ts>
    std::string buildSqlWhereClause(std::tuple<Ts...> &where, typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
    {
        auto &field = std::get<I>(where);
        std::ostringstream ss;
        ss << field.field().name() << "=?" << (I+Start+1) << (I == sizeof...(Ts)-1 ? "" : " AND ") << buildSqlWhereClause<I+1, Start, Ts...>(where);
        return ss.str();
    }

    template <typename ...Ts, std::size_t... Is>
    auto valuesFromAssignedFields_impl(const std::tuple<Ts...> &fields, std::index_sequence<Is...>)
    {
        return std::make_tuple(std::get<Is>(fields).value()...);
    };

    template <typename ...Ts>
    auto valuesFromAssignedFields(const std::tuple<Ts...> &fields)
    {
        return valuesFromAssignedFields_impl(fields, std::make_index_sequence<sizeof...(Ts)>{});
    };

public:
    SQLiteTable() = default;
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

    template <typename ...Ts>
    void createIndex (std::string name, std::tuple<Ts...> fields) {
        std::ostringstream ss;
        ss << "CREATE INDEX " << name << " ON " << mName << " ("
                                                         << buildSqlInsertFieldList<0>(fields)
                                                         << ");";
        auto stmt = newStatement(ss.str());
        execute(stmt.get());
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

    template <typename ...Ts, std::size_t...Is>
    bool insert_assign_impl(std::tuple<Ts...> fieldAndValue, std::index_sequence<Is...>) {
        return insert (std::make_tuple(std::get<Is>(fieldAndValue).field()...),
                       std::make_tuple(std::get<Is>(fieldAndValue).value()...));
    }

    template <typename ...Ts>
    bool insert(Ts... fieldAndValue) {
        return insert_assign_impl (std::make_tuple(fieldAndValue...), std::make_index_sequence<sizeof...(Ts)>{});
    }

    template <typename ...Ts, typename F, std::size_t... Is>
    void query_impl(std::tuple<Ts...> def, F resultFeedbackFunc, std::index_sequence<Is...>) {
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
    void query_impl(std::tuple<Ts...> def, std::tuple<Us...> where, F resultFeedbackFunc, std::index_sequence<Is...>) {
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

    template <typename ...Ts, typename ...Vs, typename ...Us, std::size_t...Is>
    void update_impl(std::tuple<Ts...> def, std::tuple<Vs...> values, std::tuple<Us...> where, std::index_sequence<Is...>) {
        std::ostringstream ss;
        ss << "UPDATE " << mName << " SET "
                                 << buildSqlUpdateColumnsStatement(def)
                                 << " WHERE " << buildSqlWhereClause<0,sizeof...(Ts)>(where)
                                              << ";";

        //std::cout << ss .str() << "\n";

        auto stmt = newStatement(ss.str());
        auto avalues = valuesFromAssignedFields(where);
        bindAllValues<0>(stmt.get(), values);
        bindAllValues<0,sizeof...(Ts)>(stmt.get(), avalues);
        execute(stmt.get());
    };

    template <typename ...Ts, typename ...Us, std::size_t...Is>
    void update_impl(std::tuple<Ts...> def, std::tuple<Us...> where, std::index_sequence<Is...> is) {
        update_impl(std::make_tuple(std::get<Is>(def).field()...),
                    std::make_tuple(std::get<Is>(def).value()...), where, is);
    };

    template <typename ...Ts, typename ...Us>
    void update(std::tuple<Ts...> def, std::tuple<Us...> where) {
        update_impl(def, where, std::make_index_sequence<sizeof...(Ts)>{});
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
void EXPORT SQLiteTable::bindValue<int> (SQLiteTable::Statement *stmt, int idx, int value);

template <>
void EXPORT SQLiteTable::getValue<int> (SQLiteTable::Statement *stmt, int idx, int &value);

template <>
void EXPORT SQLiteTable::bindValue<std::string> (SQLiteTable::Statement *stmt, int idx, std::string value);

template <>
void EXPORT SQLiteTable::getValue<std::string> (SQLiteTable::Statement *stmt, int idx, std::string &value);

template <>
void EXPORT SQLiteTable::bindValue<double> (SQLiteTable::Statement *stmt, int idx, double value);


template <typename ...FIELDTYPE>
inline TableDef<FIELDTYPE...> makeTableDef()
{
}

} // ns sqlite

#endif // SQLITETABLE_H
