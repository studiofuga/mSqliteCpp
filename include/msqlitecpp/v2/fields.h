/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_FIELDS_H
#define MSQLITECPP_FIELDS_H

//#include "msqlitecpp/v2/helpers/formatters.h"

#include <string>
#include <sstream>

namespace msqlitecpp {
namespace v2 {

namespace ColumnTypes {
struct Integer {
    using rawtype = int;
};
struct Real {
    using rawtype = double;
};
struct Text {
    using rawtype = std::string;
};

enum class Type {
    Integer, Real, Text, Blob
};
}

class Attribute {
    int v = 0;
public:
    Attribute() = default;

    Attribute(int value) : v(value)
    {}

    int value() const
    { return v; }

    bool operator&(const Attribute &r) const
    {
        return (v & r.value()) != 0;
    }

    Attribute &operator|=(const Attribute &r)
    {
        v = v | r.value();
        return *this;
    }

    friend Attribute operator|(const Attribute &l, const Attribute &r)
    {
        return Attribute(l.value() | r.value());
    }
};

const Attribute NoAttributes(0);
const Attribute PrimaryKey(0x01);
const Attribute NotNull(0x02);
const Attribute Unique(0x04);
const Attribute AutoIncrement(0x08);


template<typename COLUMN_TYPE>
class Column;


template<typename FIELDTYPE>
class Column {
private:
    std::string fieldName;
    Attribute attributes = NoAttributes;
    bool haveDefault = false;
    typename FIELDTYPE::rawtype defvalue;

public:
    using Type = FIELDTYPE;
    using RawType = typename Type::rawtype;

    auto rawType() -> RawType
    { return {}; }

    Column() : fieldName(), attributes()
    {}

    explicit Column(std::string name, Attribute fldattributes = NoAttributes)
            : fieldName(std::move(name)), attributes(fldattributes)
    {}

    Column(std::string name, typename FIELDTYPE::rawtype defaultValue, Attribute fldattributes = NoAttributes)
            : fieldName(std::move(name)), attributes(fldattributes), haveDefault(true), defvalue(defaultValue)
    {}

    std::string name() const
    { return fieldName; }

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

    Column<FIELDTYPE> &primaryKey()
    {
        attributes |= PrimaryKey;
        return *this;
    }

    Column<FIELDTYPE> &notNull()
    {
        attributes |= NotNull;
        return *this;
    }

    Column<FIELDTYPE> &unique()
    {
        attributes |= Unique;
        return *this;
    }

    Column<FIELDTYPE> &autoincrement()
    {
        attributes |= AutoIncrement;
        return *this;
    }
};

template<>
inline std::string Column<ColumnTypes::Integer>::sqlType() const
{ return "INTEGER"; }

template<>
inline std::string Column<ColumnTypes::Text>::sqlType() const
{ return "TEXT"; }

template<>
inline std::string Column<ColumnTypes::Real>::sqlType() const
{ return "REAL"; }

/*
namespace details {
template<typename Q>
inline std::string toString(msqlitecpp::v2::Column<Q> t)
{
    return t.name();
}

}
*/


}
}


#endif //MSQLITECPP_FIELDS_H
