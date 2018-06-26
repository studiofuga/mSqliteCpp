//
// Created by Federico Fuga on 26/06/18.
//

#ifndef SQLITE_OPERATORS_H
#define SQLITE_OPERATORS_H

#include "sqlitefielddef.h"

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

template <typename FT>
class Eq {
    const FieldDef <FT> fld;
public:
    Eq(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0){
        return details::make_oper(fld, "=", indx);
    }
};

template <typename FT>
class Ne {
    const FieldDef <FT> fld;
public:
    Ne(FieldDef <FT> f) : fld(std::move(f)) {}

    std::string operator() (int indx = 0){
        return details::make_oper(fld, "<>", indx);
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

} // op
} // sqlite


#endif //SQLITE_OPERATORS_H
