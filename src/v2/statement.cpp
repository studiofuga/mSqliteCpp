/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/statement.h"
#include "msqlitecpp/v2/exceptions.h"

#include <sqlite3.h>

namespace msqlitecpp {
namespace v2 {

struct Statement::Impl {
    std::string sqlStatement;

    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr;

    explicit Impl(std::string sql)
            : sqlStatement(std::move(sql))
    {

    }
};

Statement::Statement(char const *sql)
        : p(spimpl::make_impl<Impl>(sql))
{

}

Statement::~Statement()
{
    if (p != nullptr && p->stmt != nullptr) {
        sqlite3_finalize(p->stmt);
    }
}

bool Statement::execute(Storage &db)
{
    p->db = db.handle();
    auto r = sqlite3_prepare_v2(p->db, p->sqlStatement.c_str(), -1, &p->stmt, nullptr);
    if (r != SQLITE_OK) {
        throw Exception(p->db, p->sqlStatement);
    }

    return true;
}

}
}