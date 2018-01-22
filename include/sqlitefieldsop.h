//
// Created by Federico Fuga on 29/12/17.
//

#ifndef SQLITE_SQLITEFIELDSOP_H
#define SQLITE_SQLITEFIELDSOP_H

#include "sqlitefielddef.h"

namespace sqlite {
namespace op {

    template <typename FT>
    FieldDef<FT> count(FieldDef<FT> field) {
        return FieldDef<FT>("COUNT(" + field.name() + ")");
    }

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

    template <typename FT>
    std::string eq(const FieldDef<FT> &field) {
        return field.name() + " = ?";
    }

    template <typename FT>
    std::string ne(const FieldDef<FT> &field) {
        return field.name() + " <> ?";
    }

    template <typename FT>
    std::string lt(const FieldDef<FT> &field) {
        return field.name() + " < ?";
    }

    template <typename FT>
    std::string gt(const FieldDef<FT> &field) {
        return field.name() + " > ?";
    }

    template <typename FT>
    std::string le(const FieldDef<FT> &field) {
        return field.name() + " <= ?";
    }

    template <typename FT>
    std::string ge(const FieldDef<FT> &field) {
        return field.name() + " >= ?";
    }

    template <typename FT>
    std::string between(const FieldDef<FT> &field) {
        return field.name() + " BETWEEN ?";
    }

    template <typename FT>
    std::string like(const FieldDef<FT> &field) {
        return field.name() + " LIKE ?";
    }

    inline std::string and_(const std::string &o1, const std::string &o2) {
        return o1 + " AND " + o2;
    }

    inline std::string or_(const std::string &o1, const std::string &o2) {
        return o1 + " OR " + o2;
    }

    inline std::string not_(const std::string &o1) {
        return "NOT " + o1;
    }

    template <typename FT>
    inline FieldDef<FT> distinct(const FieldDef<FT> &field) {
        return FieldDef<FT>("DISTINCT " + field.name());
    }
}
}

#endif //SQLITE_SQLITEFIELDSOP_H
