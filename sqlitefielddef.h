#ifndef SQLITEFIELDDEF_H
#define SQLITEFIELDDEF_H

namespace sqlite {

namespace FieldType {
struct Null {};
struct Integer {};
struct Real{};
struct Text{};
struct Blob{};
}

enum class FieldAttribute : int{
    AutoIncrement = 0x01
};

template <typename FIELDTYPE>
class FieldDef {
private:
    std::string fieldName;
    int attributes = 0;

public:
    explicit FieldDef (std::string name)
        : fieldName(std::move(name)) {}

    std::string name() const { return fieldName; }
    inline std::string sqlType() const {
        static_assert(sizeof(FIELDTYPE) == 0, "Only specializations of GetGlobal can be used");
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
    return FieldDef<FIELDTYPE>(name);
}

} // ns sqlite


#endif // SQLITEFIELDDEF_H
