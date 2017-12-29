//
// Created by Federico Fuga on 29/12/17.
//

#ifndef SQLITE_SQLITEFIELDSOP_H
#define SQLITE_SQLITEFIELDSOP_H

#include "sqlitefielddef.h"

namespace sqlite {
namespace op {

    template <typename FT>
    FieldDef<FT> sum(FieldDef<FT> field) {
        return FieldDef<FT>("SUM(" + field.name() + ")");
    }

    template <typename FT>
    FieldDef<FT> avg(FieldDef<FT> field) {
        return FieldDef<FT>("AVG(" + field.name() + ")");
    }

    template <typename FT>
    FieldDef<FT> min(FieldDef<FT> field) {
        return FieldDef<FT>("MIN(" + field.name() + ")");
    }

    template <typename FT>
    FieldDef<FT> max(FieldDef<FT> field) {
        return FieldDef<FT>("MAX(" + field.name() + ")");
    }

}
}

#endif //SQLITE_SQLITEFIELDSOP_H
