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
}
}

#endif //SQLITE_SQLITEFIELDSOP_H
