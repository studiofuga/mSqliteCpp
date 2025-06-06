/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 11/06/19
 */

#ifndef MSQLITECPP_SELECTSTATEMENT_H
#define MSQLITECPP_SELECTSTATEMENT_H


#include "msqlitecpp/v2/msqlitecpp.h"
#include "msqlitecpp/v2/statement.h"
#include "msqlitecpp/v2/helpers/formatters.h"

#include <tuple>

namespace msqlitecpp {
namespace v2 {


template<typename ...FIELDS>
class SelectStatement {
    Statement statement;
    std::string tableName;
    std::tuple<FIELDS...> fields;

    std::string whereClause;

    template<std::size_t I>
    decltype(std::get<I>(fields).rawType()) getValues()
    {
        auto &field = std::get<I>(fields);
        if (field.hasDefaultValue() && statement.isNull(I)) {
            return field.defaultValue();
        }
        return statement.get<decltype(std::get<I>(fields).rawType())>(I);
    };

    template<typename F, std::size_t ...Is>
    bool execImpl(F func, std::index_sequence<Is...> const &i)
    {
        return func(getValues<Is>()...);
    };


public:
    SelectStatement(Storage &db, std::string const &tn, FIELDS... fieldlist)
            : statement(db), tableName(tn), fields(std::make_tuple(fieldlist...))
    {
        formatStatement();
    }

    std::string toString() const
    {
        return statement.toString();
    }

    template<typename F>
    void execute(F func)
    {
        statement.execute([this, func]() {
            return execImpl(func, std::make_index_sequence<sizeof...(FIELDS)>{});
        });
    }

    void where()
    {
        whereClause.clear();
        formatStatement();
    }

    void where(WhereStatement const &s)
    {
        whereClause = s.t;
        formatStatement();
    }

    template<typename ...VALUES_TYPES>
    void bind(VALUES_TYPES ... values)
    {
        statement.bind(std::make_tuple(values...));
    }

private:
    void formatStatement()
    {
        statement.set(format());
    }

    std::string format()
    {
        std::ostringstream ss;

        ss << "SELECT " << unpackFieldNames(fields)
           << " FROM " << tableName;

        if (!whereClause.empty()) {
            ss << " WHERE " << whereClause;
        }

        return ss.str();
    }
};

template<>
inline int Statement::get<int>(int idx)
{
    return getIntValue(idx);
}

template<>
inline double Statement::get<double>(int idx)
{
    return getDoubleValue(idx);
}

template<>
inline long long Statement::get<long long>(int idx)
{
    return getLongValue(idx);
}

template<>
inline unsigned long long Statement::get<unsigned long long>(int idx)
{
    return getULongValue(idx);
}

template<>
inline std::string Statement::get<std::string>(int idx)
{
    return getStringValue(idx);
}

template<typename ...FIELDS>
SelectStatement<FIELDS...> makeSelectStatement(Storage &db, std::string const &tablename, FIELDS... fields)
{
    return SelectStatement<FIELDS...>(db, tablename, fields...);
}

}
}


#endif //MSQLITECPP_SELECTSTATEMENT_H
