#ifndef SQLITEFIELDDEF_H
#define SQLITEFIELDDEF_H

#include "msqlitecpp/v1/msqlitecpp.h"

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

namespace sqlite {

namespace FieldType {
struct Integer {
    using rawtype = int;
};
struct Real {
    using rawtype = double;
};
struct Text {
    using rawtype = std::string;
};
struct Blob {
    using rawtype = std::vector<uint8_t>;
};

enum class Type {
    Integer, Real, Text, Blob
};
}

class FieldAttribute {
    int v = 0;
public:
    FieldAttribute() = default;
    FieldAttribute(int value) : v(value) {}
    int value() const { return v; }

    bool operator & (const FieldAttribute &r) const {
        return (v & r.value()) != 0;
    }
    FieldAttribute &operator |= (const FieldAttribute &r) {
        v = v | r.value();
        return *this;
    }

    friend FieldAttribute operator | (const FieldAttribute &l, const FieldAttribute &r) {
        return FieldAttribute(l.value() | r.value());
    }
};

const FieldAttribute NoAttributes(0);
const FieldAttribute PrimaryKey(0x01);
const FieldAttribute NotNull(0x02);
const FieldAttribute Unique(0x04);
const FieldAttribute AutoIncrement(0x08);

template<typename FIELDTYPE>
class FieldDef;

template<typename FIELDTYPE>
class AssignedField {
private:
    const FieldDef<FIELDTYPE> &mField;
    typename FIELDTYPE::rawtype mValue;
public:
    AssignedField(const FieldDef<FIELDTYPE> &fld, typename FIELDTYPE::rawtype val)
            : mField(fld), mValue(val)
    {}

    const FieldDef<FIELDTYPE> &field() const
    { return mField; }

    const typename FIELDTYPE::rawtype &value() const
    { return mValue; }
};

template<typename FIELDTYPE>
class FieldDef {
private:
    std::string fieldName;
    FieldAttribute attributes = NoAttributes;
    bool haveDefault = false;
    typename FIELDTYPE::rawtype defvalue;

public:
    using Type = FIELDTYPE;
    using RawType = typename Type::rawtype;

    auto rawType() -> RawType
    { return {}; }

    FieldDef() : fieldName(), attributes()
    {}

    explicit FieldDef(std::string name, FieldAttribute fldattributes = NoAttributes)
            : fieldName(std::move(name)), attributes(fldattributes)
    {}

    FieldDef(std::string name, typename FIELDTYPE::rawtype defaultValue, FieldAttribute fldattributes = NoAttributes)
            : fieldName(std::move(name)), attributes(fldattributes), haveDefault(true), defvalue(defaultValue)
    {}

    std::string name() const
    { return fieldName; }

    auto toString() const
    { return name(); }

    bool hasDefaultValue() const
    { return haveDefault; }

    auto defaultValue() const
    { return defvalue; }

    inline std::string sqlType() const
    {
        static_assert(sizeof(FIELDTYPE) == 0, "Generic version of sqlType is undefined");
        return std::string();
    }

    inline std::string sqlAttributes() const
    {
        std::ostringstream ss;

        if (attributes & PrimaryKey) {
            ss << " PRIMARY KEY";
        }
        if (attributes & AutoIncrement) {
            ss << " AUTOINCREMENT";
        }
        if (attributes & NotNull) {
            ss << " NOT NULL";
        }
        if (attributes & Unique) {
            ss << " UNIQUE";
        }

        return ss.str();
    }

    FieldDef<FIELDTYPE> &primaryKey()
    {
        attributes |= PrimaryKey;
        return *this;
    }

    FieldDef<FIELDTYPE> &notNull()
    {
        attributes |= NotNull;
        return *this;
    }

    FieldDef<FIELDTYPE> &unique()
    {
        attributes |= Unique;
        return *this;
    }

    FieldDef<FIELDTYPE> &autoincrement()
    {
        attributes |= AutoIncrement;
        return *this;
    }

    AssignedField<FIELDTYPE> assign(typename FieldDef<FIELDTYPE>::RawType value) const
    {
        return AssignedField<FIELDTYPE>(*this, value);
    }
};

template<>
inline std::string FieldDef<FieldType::Integer>::sqlType() const
{ return "INTEGER"; }

template<>
inline std::string FieldDef<FieldType::Text>::sqlType() const
{ return "TEXT"; }

template<>
inline std::string FieldDef<FieldType::Real>::sqlType() const
{ return "REAL"; }


template<typename FIELDTYPE>
inline FieldDef<FIELDTYPE> makeFieldDef(std::string name, FIELDTYPE type)
{
    (void) type;
    return FieldDef<FIELDTYPE>(name);
}

template<typename T>
inline std::string fieldName(const FieldDef<T> &field)
{
    return field.name();
}

template<typename T>
inline FieldDef<T> field(const std::string &tablename, const FieldDef<T> &field)
{
    return FieldDef<T>(tablename + "." + field.name());
}

template<typename T>
inline FieldDef<T> field(const std::string &schemaname, const std::string &tablename, const FieldDef<T> &field)
{
    return FieldDef<T>(schemaname + "." + tablename + "." + field.name());
}

template<typename U, typename T>
inline FieldDef<U> cast(const FieldDef<T> &x)
{
    return FieldDef<U>(x.name());
}

} // ns sqlite


#endif // SQLITEFIELDDEF_H
