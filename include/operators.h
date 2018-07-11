//
// Created by Federico Fuga on 26/06/18.
//

#ifndef SQLITE_OPERATORS_H
#define SQLITE_OPERATORS_H

#include "sqlitefielddef.h"

#include <boost/optional.hpp>

#include <iostream>

namespace sqlite {
namespace operators {

namespace details {
template<typename FT>
std::string make_oper(const FieldDef <FT> &fld, std::string oper, size_t indx)
{
    std::ostringstream ss;
    ss << fld.name() << " " << oper << " ?" << indx + 1;
    return ss.str();
}

}

template <typename OP, typename T>
std::string format(int indx, const OP& op, boost::optional<T> t) {
    if (t.is_initialized())
        return op(indx);
    return std::string();
}

template <typename FT>
class Eq {
    const FieldDef <FT> fld;
public:
    Eq(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0) const {
        return details::make_oper(fld, "=", indx);
    }

    template<typename T>
    std::string format(int indx, boost::optional<T> t) {
        if (t.is_initialized())
            return (*this)(indx);
        return std::string();
    }
};

template <typename FT>
class Ne {
    const FieldDef <FT> fld;
public:
    Ne(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0) const {
        return details::make_oper(fld, "<>", indx);
    }

    template<typename T>
    std::string format(int indx, boost::optional<T> t) {
        if (t.is_initialized())
            return (*this)(indx);
        return std::string();
    }
};

/** @brief Implements the Less Than "<" operator with optionals */
template <typename FT>
class Lt {
    const FieldDef <FT> fld;
public:
    Lt(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0) const {
        return details::make_oper(fld, "<", indx);
    }

    template<typename T>
    std::string format(int indx, boost::optional<T> t) {
        if (t.is_initialized())
            return (*this)(indx);
        return std::string();
    }
};

/** @brief Implements the Less or Equal "<=" operator with optionals */
template <typename FT>
class Le {
    const FieldDef <FT> fld;
public:
    Le(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0) const {
        return details::make_oper(fld, "<=", indx);
    }

    template<typename T>
    std::string format(int indx, boost::optional<T> t) {
        if (t.is_initialized())
            return (*this)(indx);
        return std::string();
    }
};

/** @brief Implements the Greater Than ">" operator with optionals */
template <typename FT>
class Gt {
    const FieldDef <FT> fld;
public:
    Gt(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0) const {
        return details::make_oper(fld, ">", indx);
    }

    template<typename T>
    std::string format(int indx, boost::optional<T> t) {
        if (t.is_initialized())
            return (*this)(indx);
        return std::string();
    }
};

/** @brief Implements the Greater than or Equal to ">=" operator with optionals */
template <typename FT>
class Ge {
    const FieldDef <FT> fld;
public:
    Ge(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0) const {
        return details::make_oper(fld, ">=", indx);
    }

    template<typename T>
    std::string format(int indx, boost::optional<T> t) {
        if (t.is_initialized())
            return (*this)(indx);
        return std::string();
    }
};

template <typename ...OPS>
class And {
    const std::tuple<OPS...> fields;

    template <size_t I, typename std::enable_if<I == sizeof...(OPS), int>::type = 0 >
    std::string opImpl(int idx) {
        return std::string();
    }

    template <size_t I, typename std::enable_if<I < sizeof...(OPS), int>::type = 0 >
    std::string opImpl(int idx) {
        auto &f = std::get<I>(fields);
        return (I == 0 ? "" : " AND ") + f(I+idx) + opImpl<I+1>(idx);
    }

    template <size_t I, typename ...Ts, typename std::enable_if<I == sizeof...(Ts), int>::type = 0 >
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0) {
        return std::string();
    }
    template <size_t I, typename ...Ts, typename std::enable_if<I < sizeof...(Ts), int>::type = 0 >
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0) {
        auto &value = std::get<I>(ts);
        auto &field = std::get<I>(fields);
        std::ostringstream ss;
        if (value.is_initialized()) {
            ss << (count == 0 ? "" : " AND ") + field(I+idx);
            ++count;
        }
        ss << formatImpl<I+1>(idx, ts, count);
        return  ss.str();
    }

public:
    And (OPS... f) : fields(std::make_tuple(f...)) {
    }

    std::string operator() (int idx = 0){
        return opImpl<0>(idx);
    }

    template<typename ...T>
    std::string format(int indx, boost::optional<T>... t) {
        return formatImpl<0>(indx, std::make_tuple(t...));
    }
};

// TODO avoid duplicating this class. The only changes is the " AND " string.
template <typename ...OPS>
class Or {
    const std::tuple<OPS...> fields;

    template <size_t I, typename std::enable_if<I == sizeof...(OPS), int>::type = 0 >
    std::string opImpl(int idx) {
        return std::string();
    }

    template <size_t I, typename std::enable_if<I < sizeof...(OPS), int>::type = 0 >
    std::string opImpl(int idx) {
        auto &f = std::get<I>(fields);
        return (I == 0 ? "" : " OR ") + f(I+idx) + opImpl<I+1>(idx);                // HERE
    }

    template <size_t I, typename ...Ts, typename std::enable_if<I == sizeof...(Ts), int>::type = 0 >
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0) {
        return std::string();
    }
    template <size_t I, typename ...Ts, typename std::enable_if<I < sizeof...(Ts), int>::type = 0 >
    std::string formatImpl(int idx, std::tuple<Ts...> ts, int count = 0) {
        auto &value = std::get<I>(ts);
        auto &field = std::get<I>(fields);
        std::ostringstream ss;
        if (value.is_initialized()) {
            ss << (count == 0 ? "" : " OR ") + field(I+idx);        // And HERE
            ++count;
        }
        ss << formatImpl<I+1>(idx, ts, count);
        return  ss.str();
    }

public:
    Or (OPS... f) : fields(std::make_tuple(f...)) {
    }

    std::string operator() (int idx = 0){
        return opImpl<0>(idx);
    }

    template<typename ...T>
    std::string format(int indx, boost::optional<T>... t) {
        return formatImpl<0>(indx, std::make_tuple(t...));
    }
};

template<typename FT>
Ne<FT> ne(const FieldDef <FT> &field)
{
    return Ne<FT>(field);
}

template <typename FT>
Eq<FT> eq(FieldDef<FT> f) {
    return Eq<FT>(f);
}

template <typename FT>
Lt<FT> lt(FieldDef<FT> f) {
    return Lt<FT>(f);
}

template <typename FT>
Le<FT> le(FieldDef<FT> f) {
    return Le<FT>(f);
}

template <typename FT>
Gt<FT> gt(FieldDef<FT> f) {
    return Gt<FT>(f);
}

template <typename FT>
Ge<FT> ge(FieldDef<FT> f) {
    return Ge<FT>(f);
}

template<typename ...OPS>
And<OPS...> and_(OPS... ops) {
    return And<OPS...>(ops...);
}

template<typename ...OPS>
Or<OPS...> or_(OPS... ops) {
    return Or<OPS...>(ops...);
}

} // op
} // sqlite


#endif //SQLITE_OPERATORS_H
