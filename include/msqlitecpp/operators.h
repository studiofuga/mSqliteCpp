//
// Created by Federico Fuga on 26/06/18.
//

#ifndef SQLITE_OPERATORS_H
#define SQLITE_OPERATORS_H

#include "msqlitecpp/sqlitefielddef.h"

#if defined(WITH_BOOST)
#include <boost/optional.hpp>
#endif

#include <iostream>
#include <string>

namespace sqlite {
namespace operators {

namespace details {
template<typename FT>
std::string make_oper(const FT &fld, std::string oper, size_t indx)
{
    std::ostringstream ss;
    ss << fld.name() << " " << oper << " ?" << indx + 1;
    return ss.str();
}

}

#if defined(WITH_BOOST)
template<typename OP, typename T>
std::string format(int indx, const OP &op, boost::optional<T> t)
{
    if (t.is_initialized()) {
        return op(indx);
    }
    return std::string();
}
#endif

template<typename FT>
class UnaryOp {
    const std::string sep;
    const FT fld;
public:
    UnaryOp(FT f, const std::string &separator)
            : sep(separator),
              fld(std::move(f))
    {}

    std::string operator()(int indx = 0) const
    {
        return details::make_oper(fld, sep, indx);
    }

#if defined(WITH_BOOST)
    template<typename T>
    std::string format(int indx, boost::optional<T> t)
    {
        if (t.is_initialized()) {
            return (*this)(indx);
        }
        return std::string();
    }
#endif
};

template<typename ...OPS>
class And {
    const std::tuple<OPS...> fields;

    template<size_t I, typename std::enable_if<I == sizeof...(OPS), int>::type = 0>
    std::string opImpl(int idx)
    {
        return std::string();
    }

    template<size_t I, typename std::enable_if<I < sizeof...(OPS), int>::type = 0>
    std::string opImpl(int idx)
    {
        auto &f = std::get<I>(fields);
        return (I == 0 ? "" : " AND ") + f(I + idx) + opImpl<I + 1>(idx);
    }

    template<size_t I, typename ...Ts, typename std::enable_if<I == sizeof...(Ts), int>::type = 0>
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0)
    {
        return std::string();
    }

    template<size_t I, typename ...Ts, typename std::enable_if<I < sizeof...(Ts), int>::type = 0>
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0)
    {
        auto &value = std::get<I>(ts);
        auto &field = std::get<I>(fields);
        std::ostringstream ss;
        if (value.is_initialized()) {
            ss << (count == 0 ? "" : " AND ") + field(I + idx);
            ++count;
        }
        ss << formatImpl<I + 1>(idx, ts, count);
        return ss.str();
    }

public:
    And(OPS... f) : fields(std::make_tuple(f...))
    {
    }

    std::string operator()(int idx = 0)
    {
        return opImpl<0>(idx);
    }

#if defined(WITH_BOOST)
    template<typename ...T>
    std::string format(int indx, boost::optional<T>... t)
    {
        return formatImpl<0>(indx, std::make_tuple(t...));
    }
#endif
};

// TODO avoid duplicating this class. The only changes is the " AND " string.
template<typename ...OPS>
class Or {
    const std::tuple<OPS...> fields;

    template<size_t I, typename std::enable_if<I == sizeof...(OPS), int>::type = 0>
    std::string opImpl(int idx)
    {
        return std::string();
    }

    template<size_t I, typename std::enable_if<I < sizeof...(OPS), int>::type = 0>
    std::string opImpl(int idx)
    {
        auto &f = std::get<I>(fields);
        return (I == 0 ? "" : " OR ") + f(I + idx) + opImpl<I + 1>(idx);                // HERE
    }

    template<size_t I, typename ...Ts, typename std::enable_if<I == sizeof...(Ts), int>::type = 0>
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0)
    {
        return std::string();
    }

    template<size_t I, typename ...Ts, typename std::enable_if<I < sizeof...(Ts), int>::type = 0>
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0)
    {
        auto &value = std::get<I>(ts);
        auto &field = std::get<I>(fields);
        std::ostringstream ss;
        if (value.is_initialized()) {
            ss << (count == 0 ? "" : " OR ") + field(I + idx);        // And HERE
            ++count;
        }
        ss << formatImpl<I + 1>(idx, ts, count);
        return ss.str();
    }

public:
    Or(OPS... f) : fields(std::make_tuple(f...))
    {
    }

    std::string operator()(int idx = 0)
    {
        return opImpl<0>(idx);
    }

#if defined(WITH_BOOST)
    template<typename ...T>
    std::string format(int indx, boost::optional<T>... t)
    {
        return formatImpl<0>(indx, std::make_tuple(t...));
    }
#endif
};

template <typename FT>
UnaryOp<FT> makeOper(const FT &field, std::string separator) {
    return UnaryOp<FT>(field, separator);
}

template<typename FT>
UnaryOp<FT> ne(const FT &field)
{
    return makeOper(field, "<>");
}

template<typename FT>
UnaryOp<FT> eq(FT field)
{
    return makeOper(field, "=");
}

template<typename FT>
UnaryOp<FT> lt(FT f)
{
    return makeOper(f, "<");
}

template<typename FT>
UnaryOp<FT> le(FT f)
{
    return makeOper(f, "<=");
}

template<typename FT>
UnaryOp<FT> gt(FT f)
{
    return makeOper(f, ">");
}

template<typename FT>
UnaryOp<FT> ge(FT f)
{
    return makeOper(f, ">=");
}

template<typename ...OPS>
And<OPS...> and_(OPS... ops)
{
    return And<OPS...>(ops...);
}

template<typename ...OPS>
Or<OPS...> or_(OPS... ops)
{
    return Or<OPS...>(ops...);
}

} // op
} // sqlite


#endif //SQLITE_OPERATORS_H
