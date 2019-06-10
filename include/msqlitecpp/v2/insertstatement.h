/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_INSERTSTATEMENT_H
#define MSQLITECPP_INSERTSTATEMENT_H

#include "msqlitecpp/v2/msqlitecpp.h"
#include "msqlitecpp/v2/statement.h"
#include "msqlitecpp/v2/helpers/formatters.h"

#include <tuple>

namespace msqlitecpp {
namespace v2 {

template<typename ...FIELDS>
class InsertStatement {
    Statement statement;
    std::tuple<FIELDS...> fields;

public:
    InsertStatement(Storage &db, std::string const &tablename, FIELDS... fieldlist)
            : statement(db), fields(std::make_tuple(fieldlist...))
    {
        statement.set(format(tablename, fields));
    }

    template<typename ...VALUES_TYPES>
    void insert(VALUES_TYPES ... values)
    {
        statement.bind(std::make_tuple(values...));
        statement.execute();
    }

    std::string toString() const
    {
        return statement.toString();
    }

private:
    std::string format(std::string const &tablename, std::tuple<FIELDS...> const &fields)
    {
        std::ostringstream ss;

        ss << "INSERT INTO " << tablename << "("
           << unpackFieldNames(fields) << ") VALUES("
           << unpackFieldPlaceholders(fields) << ")";

        return ss.str();
    }
};

template<typename ...FIELDS>
static InsertStatement<FIELDS...> makeInsertStatement(Storage &db, std::string const &tablename, FIELDS... fields)
{
    return InsertStatement<FIELDS...>(db, tablename, fields...);
}


}
}

#endif //MSQLITECPP_INSERTSTATEMENT_H
