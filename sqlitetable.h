#ifndef SQLITETABLE_H
#define SQLITETABLE_H

#include <string>
#include <memory>
#include <tuple>

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
    return field.name() + " " + field.sqlType() + (I == sizeof...(Ts)-1 ? "" : ",") + "\n" +
                                                                                    buildSqlCreateString<I+1, Ts...>(def);
};


class SQLiteTable
{
    std::weak_ptr<SQLiteStorage> mdb;
    std::string mName;
public:
    SQLiteTable(std::shared_ptr<SQLiteStorage> db, std::string name);
    virtual ~SQLiteTable() noexcept;

    virtual bool create();
    bool createFromSQLString (std::string query);
    //virtual bool update() = 0;

    std::string name() const { return mName; }

protected:
    std::shared_ptr<SQLiteStorage> db();
    // helper functions here

    virtual std::string getCreateDefinition() { return std::string(); }

    template <typename ...FIELDDEF>
    static SQLiteTable create (std::shared_ptr<SQLiteStorage> db, std::string name, std::tuple<FIELDDEF...> &&def) {
        SQLiteTable table(db, name);

        table.create(def);

        return table;
    }
};


template <typename ...FIELDTYPE>
inline TableDef<FIELDTYPE...> makeTableDef()
{
}

} // ns sqlite

#endif // SQLITETABLE_H
