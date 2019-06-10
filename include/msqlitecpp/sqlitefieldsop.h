//
// Created by Federico Fuga on 29/12/17.
//

#ifndef SQLITE_SQLITEFIELDSOP_H
#define SQLITE_SQLITEFIELDSOP_H

#include "msqlitecpp/sqlitefielddef.h"
#include "msqlitecpp/sqlformatters_helpers.h"

namespace sqlite {
namespace op {

namespace details {
template<size_t I, typename ...OP, typename std::enable_if<I == sizeof...(OP), int>::type = 0>
std::string unpackImpl(const char *, std::tuple<OP...>)
{
    return std::string();
};

template<size_t I, typename ...OP, typename std::enable_if<I < sizeof...(OP), int>::type = 0>
std::string unpackImpl(const char *sep, std::tuple<OP...> op)
{
   return sqlite::helpers::toString(std::get<I>(op)) + (I < sizeof...(OP)-1 ? sep : "") + unpackImpl<I + 1>(sep, op);
};
}

template<typename FT>
FieldDef <FT> count(FieldDef <FT> field)
{
    return FieldDef<FT>("COUNT(" + field.name() + ")");
}

template<typename FT>
FieldDef <FT> sum(FieldDef <FT> field)
{
    return FieldDef<FT>("SUM(" + field.name() + ")");
}

template<typename FT>
FieldDef <FT> avg(FieldDef <FT> field)
{
    return FieldDef<FT>("AVG(" + field.name() + ")");
}

template<typename FT>
FieldDef <FT> min(FieldDef <FT> field)
{
    return FieldDef<FT>("MIN(" + field.name() + ")");
}

template<typename FT>
FieldDef <FT> max(FieldDef <FT> field)
{
    return FieldDef<FT>("MAX(" + field.name() + ")");
}

template<typename FT>
std::string eq(const FieldDef <FT> &field)
{
    return field.name() + " = ?";
}

template<typename FT>
std::string ne(const FieldDef <FT> &field)
{
    return field.name() + " <> ?";
}

template <typename FT>
class eq_ {
    const FieldDef <FT> fld;
public:
    eq_(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0){
        std::ostringstream ss;
        ss << fld.name() << " = ?" << indx+1;
        return ss.str();
    }
};

template <typename FT>
eq_<FT> makeEq(FieldDef<FT> f) {
    return eq_<FT>(f);
}

template<typename FT>
std::string lt(const FieldDef <FT> &field)
{
    return field.name() + " < ?";
}

template<typename FT>
std::string gt(const FieldDef <FT> &field)
{
    return field.name() + " > ?";
}

template<typename FT>
std::string le(const FieldDef <FT> &field)
{
    return field.name() + " <= ?";
}

template<typename FT>
std::string ge(const FieldDef <FT> &field)
{
    return field.name() + " >= ?";
}

template<typename FT>
std::string between(const FieldDef <FT> &field)
{
    return field.name() + " BETWEEN ?";
}

template<typename FT>
std::string like(const FieldDef <FT> &field)
{
    return field.name() + " LIKE ?";
}

template <typename ...OP>
inline std::string and_(OP... op)
{
    return "(" + details::unpackImpl<0>(" AND ", std::make_tuple(op...)) + ")";
}

template <typename ...OP>
inline std::string or_(OP... op)
{
    return "(" + details::unpackImpl<0>(" OR ", std::make_tuple(op...)) + ")";
}

inline std::string not_(const std::string &o1)
{
    return "NOT " + o1;
}

template<typename FT>
inline FieldDef <FT> distinct(const FieldDef <FT> &field)
{
    return FieldDef<FT>("DISTINCT " + field.name());
}
}
}

#endif //SQLITE_SQLITEFIELDSOP_H
