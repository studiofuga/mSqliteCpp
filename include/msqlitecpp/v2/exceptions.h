/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_EXCEPTIONS_H
#define MSQLITECPP_EXCEPTIONS_H

#include "msqlitecpp/v2/msqlitecpp.h"

#include <sqlite3.h>

#include <stdexcept>
#include <sstream>

struct sqlite3;

namespace msqlitecpp {
namespace v2 {

class EXPORT_MSQLITEV2 Exception : public std::runtime_error {
    std::string mErrmsg;
    int mCode;
public:
    explicit Exception(sqlite3 *db) : std::runtime_error("")
    {
        mErrmsg = sqlite3_errmsg(db);
        mCode = sqlite3_errcode(db);
    }

    explicit Exception(sqlite3 *db, std::string details) : std::runtime_error("")
    {
        std::ostringstream ss;
        ss << details << ": " <<
           sqlite3_errmsg(db);
        mErrmsg = ss.str();
        mCode = sqlite3_errcode(db);
    }

    explicit Exception(const Exception &x, std::string details) : std::runtime_error("")
    {
        std::ostringstream ss;
        ss << details << ": " << x.what();
        mErrmsg = ss.str();
        mCode = x.code();
    }

    const char *what() const noexcept override
    {
        return mErrmsg.c_str();
    }

    int code() const
    {
        return mCode;
    }

    static void throwIfNotOk(int returnCode, sqlite3 *db)
    {
        if (returnCode != SQLITE_OK) {
            throw Exception(db);
        }
    }

};

}
}

#endif //MSQLITECPP_EXCEPTIONS_H
