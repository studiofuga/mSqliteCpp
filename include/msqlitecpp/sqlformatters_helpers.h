/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 15/05/18
 */

#ifndef SQLITE_SQLFORMATTERS_HELPERS_H
#define SQLITE_SQLFORMATTERS_HELPERS_H

#include "msqlitecpp/sqlitefielddef.h"

#include <string>

namespace sqlite {
namespace helpers {

template<typename T>
std::string toString(T t)
{
    return t.toString();
}

template<>
inline std::string toString(std::string t)
{
    return t;
}

template<>
inline std::string toString(const char *t)
{
    return std::string(t);
}

template<typename Q>
inline std::string toString(sqlite::FieldDef<Q> t)
{
    return t.name();
}

}
}

#endif //SQLITE_SQLFORMATTERS_HELPERS_H
