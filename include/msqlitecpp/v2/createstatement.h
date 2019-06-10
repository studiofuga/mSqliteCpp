/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_CREATESTATEMENT_H
#define MSQLITECPP_CREATESTATEMENT_H

#include "msqlitecpp/v2/msqlitecpp.h"
#include "msqlitecpp/v2/statement.h"
#include "msqlitecpp/v2/helpers/formatters.h"

#include <tuple>

namespace msqlitecpp {
namespace v2 {

template<typename ...FIELDS>
class CreateStatement {
    Statement statement;
public:
    CreateStatement(Storage &db, std::string const &tablename, FIELDS... fieldlist)
            : statement(db)
    {
        auto fields = std::make_tuple(fieldlist...);
        statement.set(format(tablename, fields));
    }

    void execute()
    {
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

        ss << "CREATE TABLE " << tablename << "("
           << unpackFieldDefinitions(fields)
           << ")";

        return ss.str();
    }

};

template<typename ...FIELDS>
static CreateStatement<FIELDS...> makeCreateStatement(Storage &db, std::string const &tablename, FIELDS... fields)
{
    return CreateStatement<FIELDS...>(db, tablename, fields...);
}


}
}

#endif //MSQLITECPP_CREATESTATEMENT_H
