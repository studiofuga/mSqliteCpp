/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_STATEMENT_H
#define MSQLITECPP_STATEMENT_H

#include "msqlitecpp/v2/msqlitecpp.h"
#include "msqlitecpp/v2/storage.h"

#include "msqlitecpp/utils/spimpl.h"

namespace msqlitecpp {
namespace v2 {

class EXPORT_MSQLITEV2 Statement {
private:
    struct Impl;

    spimpl::impl_ptr<Impl> p;
public:
    explicit Statement(char const *sql);

    static Statement make(char const *sql)
    {
        return Statement(sql);
    }

    bool execute(Storage &handle);

    virtual ~Statement();
};

}
}

#endif //MSQLITECPP_STATEMENT_H
