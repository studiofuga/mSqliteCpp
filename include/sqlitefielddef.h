#ifndef SQLITEFIELDDEF_H
#define SQLITEFIELDDEF_H

#include "msqlitecpp.h"

#include <string>
#include <vector>
#include <sstream>

namespace sqlite {

namespace FieldType {
struct Integer { using rawtype = int; };
struct Real{ using rawtype = double; };
struct Text{ using rawtype = std::string; };
struct Blob{ using rawtype = std::vector<uint8_t>; };

    enum class Type { Integer, Real, Text, Blob };
}

enum FieldAttribute : int{
    PrimaryKey = 0x01,
    NotNull = 0x02,
    Unique = 0x04,
    AutoIncrement = 0x08
};

template <typename FIELDTYPE>
class FieldDef;

template <typename FIELDTYPE>
class AssignedField {
private:
    const FieldDef<FIELDTYPE> &mField;
    typename FIELDTYPE::rawtype mValue;
public:
    AssignedField(const FieldDef<FIELDTYPE> &fld, typename FIELDTYPE::rawtype val)
            : mField(fld), mValue(val) {}

    const FieldDef<FIELDTYPE> &field() const { return mField; }
    const typename FIELDTYPE::rawtype &value() const { return mValue; }
};

template <typename FIELDTYPE>
class FieldDef {
private:
    std::string fieldName;
    int attributes = 0;

public:
    using Type = FIELDTYPE;
    using RawType = typename Type::rawtype;

    auto rawType() -> RawType { return {}; }

    explicit FieldDef (std::string name)
        : fieldName(std::move(name)) {}

    std::string name() const { return fieldName; }
    inline std::string sqlType() const {
        static_assert(sizeof(FIELDTYPE) == 0, "Generic version of sqlType is undefined");
        return std::string();
    }

    inline std::string sqlAttributes() const {
        std::ostringstream ss;

        if (attributes & FieldAttribute::PrimaryKey)
            ss << " PRIMARY KEY";
        if (attributes & FieldAttribute::AutoIncrement)
            ss << " AUTOINCREMENT";
        if (attributes & FieldAttribute::NotNull)
            ss << " NOT NULL";
        if (attributes & FieldAttribute::Unique)
            ss << " UNIQUE";

        return ss.str();
    }

    FieldDef<FIELDTYPE> &primaryKey() {
        attributes |= FieldAttribute::PrimaryKey;
        return *this;
    }
    FieldDef<FIELDTYPE> &notNull() {
        attributes |= FieldAttribute::NotNull;
        return *this;
    }
    FieldDef<FIELDTYPE> &unique() {
        attributes |= FieldAttribute::Unique;
        return *this;
    }
    FieldDef<FIELDTYPE> &autoincrement() {
        attributes |= FieldAttribute::AutoIncrement;
        return *this;
    }

    AssignedField<FIELDTYPE> assign(typename FieldDef<FIELDTYPE>::RawType value) const {
        return AssignedField<FIELDTYPE>(*this, value);
    }
};

template <>
inline std::string FieldDef<FieldType::Integer>::sqlType() const { return "INTEGER"; }

template <>
inline std::string FieldDef<FieldType::Text>::sqlType() const { return "TEXT"; }

template <>
inline std::string FieldDef<FieldType::Real>::sqlType() const { return "REAL"; }


template <typename FIELDTYPE>
inline FieldDef<FIELDTYPE> makeFieldDef(std::string name, FIELDTYPE type) {
    (void)type;
    return FieldDef<FIELDTYPE>(name);
}

template <typename T>
inline std::string fieldName(const FieldDef<T> &field) {
    return field.name();
}

template <typename T>
inline FieldDef<T> field(const std::string &tablename, const FieldDef<T> &field) {
    return FieldDef<T>(tablename + "." + field.name());
}

template <typename T>
inline FieldDef<T> field(const std::string &schemaname, const std::string &tablename, const FieldDef<T> &field) {
    return FieldDef<T>(schemaname + "." + tablename + "." + field.name());
}

template <typename U, typename T>
inline FieldDef<U> cast(const FieldDef<T> &x) {
    return FieldDef<U>(x.name());
}

} // ns sqlite


#endif // SQLITEFIELDDEF_H
