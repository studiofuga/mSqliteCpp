//
// Created by Federico Fuga on 29/12/17.
//

#ifndef SQLITE_SQLITESTATEMENTFORMATTERS_H
#define SQLITE_SQLITESTATEMENTFORMATTERS_H

#include "sqlitefielddef.h"
#include <sstream>
#include <tuple>

#if defined(WITH_BOOST)
#include "boost/optional.hpp"
#endif

namespace sqlite {
namespace statements {

namespace details {

template<typename T>
std::string toString(T t)
{
    return t.toString();
}

template<>
inline std::string toString(std::string t)
{
    return t;
}

template<>
inline std::string toString(const char *t)
{
    return std::string(t);
}

template<typename Q>
inline std::string toString(sqlite::FieldDef<Q> t)
{
    return t.name();
}

#if defined(WITH_BOOST)
template <typename F, typename T>
inline std::string toString(const F &field, boost::optional<T> v)
{
    if (v)
        return toString(field);
    return std::string{};
}

template <typename T>
inline std::string toString(boost::optional<T> v)
{
    if (v)
        return toString(v.value());
    return std::string{};
}
#endif

template <typename T>
std::string formatFieldName (const T&t, const std::string &sep)
{
    return sep + toString(t);
}

template <typename F, typename T>
std::string formatFieldName (const F&f, const T &t,const std::string &sep)
{
    return sep + toString(f);
}

#if defined(WITH_BOOST)
template <typename T>
std::string formatFieldName (const boost::optional<T> &t, const std::string &sep)
{
    if (t)
        return sep + toString(t);
    return std::string{};
}

template <typename F, typename T>
std::string formatFieldName (const F& field, const boost::optional<T> &t, const std::string &sep)
{
    if (t)
        return sep + toString(field);
    return std::string{};
}
#endif

template<size_t I, typename ...Ts>
std::string
unpackFieldNames_impl(const std::tuple<Ts...> &def, size_t count = 0,
                      typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    (void) def;
    (void) count;
    return std::string();
}

/// @brief builds a Field list part of a SQL Insert statement
template<size_t I, typename ...Ts>
std::string
unpackFieldNames_impl(const std::tuple<Ts...> &def, size_t count = 0,
                      typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto fld = formatFieldName(std::get<I>(def), (count > 0 ? "," : ""));
    if (!fld.empty())
        ++count;
    return fld + unpackFieldNames_impl<I + 1, Ts...>(def, count);
}

template<size_t I, typename ...Ts, typename ...Vs>
std::string
unpackFieldNamesOpt_impl(const std::tuple<Ts...> &def,
                         const std::tuple<Vs...> &v,
                         size_t count = 0,
                         typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    (void) def;
    (void) count;
    return std::string();
}

/// @brief builds a Field list part of a SQL Insert statement
template<size_t I, typename ...Ts, typename ...Vs>
std::string
unpackFieldNamesOpt_impl(const std::tuple<Ts...> &def,
                         const std::tuple<Vs...> &v,
                         size_t count = 0,
                         typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto fld = formatFieldName(std::get<I>(def), std::get<I>(v), (count > 0 ? "," : ""));
    if (!fld.empty())
        ++count;
    return fld + unpackFieldNamesOpt_impl<I + 1, Ts...>(def, v, count);
}

template <typename T>
std::string formatFieldPlaceholder3 (const T&t, const std::string &sep, size_t I = 0)
{
    std::ostringstream ss;
    ss << sep << "?" << I+1;
    return ss.str();
}

template <typename F, typename T>
std::string formatFieldPlaceholder4 (const F &f, const T&t, const std::string &sep, size_t I = 0)
{
    std::ostringstream ss;
    ss << sep << "?" << I+1;
    return ss.str();
}

#if defined(WITH_BOOST)
template <typename T>
std::string formatFieldPlaceholder3 (const boost::optional<T> &t, const std::string &sep, size_t I = 0)
{
    if (t) {
        std::ostringstream ss;
        ss << sep << "?" << I+1;
        return ss.str();
    }
    return std::string{};
}

template <typename F, typename T>
std::string formatFieldPlaceholder4 (const F &f, const boost::optional<T> &t, const std::string &sep, size_t I = 0)
{
    if (t) {
        std::ostringstream ss;
        ss << sep << "?" << I+1;
        return ss.str();
    }
    return std::string{};
}
#endif

template<size_t I, typename ...Ts>
std::string
unpackFieldPlaceholders_impl(const std::tuple<Ts...> &def, size_t count = 0,
                             typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    (void) def;
    (void) count;
    return std::string();
}

/// @brief builds a Field list part of a SQL Insert statement
template<size_t I, typename ...Ts>
std::string
unpackFieldPlaceholders_impl(const std::tuple<Ts...> &def, size_t count = 0,
                             typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto fld = formatFieldPlaceholder3(std::get<I>(def), (count > 0 ? "," : ""),I);
    if (!fld.empty())
        ++count;
    return  fld + unpackFieldPlaceholders_impl<I + 1, Ts...>(def, count);
}

template<size_t I, typename ...Ts, typename ...Vs>
std::string
unpackFieldPlaceholdersOpt_impl(const std::tuple<Ts...> &def,
                                const std::tuple<Vs...> &values,
                                size_t count = 0,
                                typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    (void) def;
    (void) count;
    (void) values;
    return std::string();
}

/// @brief builds a Field list part of a SQL Insert statement
template<size_t I, typename ...Ts, typename ...Vs>
std::string
unpackFieldPlaceholdersOpt_impl(const std::tuple<Ts...> &def,
                                const std::tuple<Vs...> &values,
                                size_t count = 0,
                                typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto fld = formatFieldPlaceholder4(std::get<I>(def), std::get<I>(values), (count > 0 ? "," : ""),I);
    if (!fld.empty())
        ++count;
    return  fld + unpackFieldPlaceholdersOpt_impl<I + 1, Ts...>(def, values, count);
}


template <typename T>
std::string formatFieldNameAndPlaceholder (const T&t, const std::string &sep, size_t I = 0)
{
    std::ostringstream ss;
    ss << sep << toString(t) << " = ?" << I+1;
    return ss.str();
}

#if defined(WITH_BOOST)
template <typename T>
std::string formatFieldNameAndPlaceholder (const boost::optional<T> &t, const std::string &sep, size_t I = 0)
{
    if (t) {
        std::ostringstream ss;
        ss << sep << toString(t) << " = ?" << I+1;
        return ss.str();
    }
    return std::string{};
}
template <typename F, typename T>
std::string formatFieldNameAndPlaceholderOpt (const F &f, const boost::optional<T> &t, const std::string &sep, size_t I = 0)
{
    if (t) {
        std::ostringstream ss;
        ss << sep << toString(f) << " = ?" << I+1;
        return ss.str();
    }
    return std::string{};
}
#endif

/// @brief Ends the recursion of unpackFieldsAndPlaceholders_impl()
template<size_t I, typename ...Ts>
std::string unpackFieldsAndPlaceholders_impl(const std::tuple<Ts...> &, size_t = 0,
                                             typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    return std::string();
}
/// @brief unpacks the fields into a list of strings in the form (FieldName = ?[,FieldName = ?...])
template<size_t I, typename ...Ts>
std::string unpackFieldsAndPlaceholders_impl(const std::tuple<Ts...> &def, size_t count = 0,
                                             typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto fld = formatFieldNameAndPlaceholder(std::get<I>(def), (count > 0 ? "," : ""),I);
    if (!fld.empty())
        ++count;

    auto const &field = std::get<I>(def);
    return fld + unpackFieldsAndPlaceholders_impl<I + 1>(def, count);
}


/// @brief Ends the recursion of unpackFieldsAndPlaceholders_impl()
template<size_t I, typename ...Ts, typename ...Vs>
std::string unpackFieldsAndPlaceholdersOpt_impl(const std::tuple<Ts...> &,
                                                const std::tuple<Vs...> &v,
                                                size_t = 0,
                                                typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    return std::string();
}

/// @brief unpacks the fields into a list of strings in the form (FieldName = ?[,FieldName = ?...])
template<size_t I, typename ...Ts, typename ...Vs>
std::string unpackFieldsAndPlaceholdersOpt_impl(const std::tuple<Ts...> &def,
                                                const std::tuple<Vs...> &values,
                                                size_t count = 0,
                                                typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto fld = formatFieldNameAndPlaceholderOpt(std::get<I>(def), std::get<I>(values), (count > 0 ? "," : ""),I);
    if (!fld.empty())
        ++count;

    return fld + unpackFieldsAndPlaceholdersOpt_impl<I + 1>(def, values, count);
}


template<size_t I, typename ...Ts>
std::string unpackFieldDefinitions_impl(const std::tuple<Ts...> &,
                                        typename std::enable_if<I == sizeof...(Ts)>::type * = 0)
{
    return std::string();
}

template<size_t I, typename ...Ts>
std::string unpackFieldDefinitions_impl(const std::tuple<Ts...> &def,
                                        typename std::enable_if<I < sizeof...(Ts)>::type * = 0)
{
    auto const &field = std::get<I>(def);
    return toString(field) + " " + field.sqlType() + field.sqlAttributes()
           + (I == sizeof...(Ts) - 1 ? "" : ", ")
           + unpackFieldDefinitions_impl<I + 1>(def);
}
}

template<typename ...Ts>
std::string unpackFieldNames(Ts... def)
{
    return details::unpackFieldNames_impl<0>(std::make_tuple(def...), 0);
}

template<typename ...Ts>
std::string unpackFieldNames(std::tuple<Ts...> def)
{
    return details::unpackFieldNames_impl<0>(def);
}

template<typename ...Ts, typename ...Vs>
std::string unpackFieldNamesOpt(std::tuple<Ts...> def, std::tuple<Vs...> values)
{
    return details::unpackFieldNamesOpt_impl<0>(def, values);
}


template<typename ...Ts>
std::string unpackFieldPlaceholders(Ts... def)
{
    return details::unpackFieldPlaceholders_impl<0>(std::make_tuple(def...));
}

template<typename ...Ts>
std::string unpackFieldPlaceholders(std::tuple<Ts...> def)
{
    return details::unpackFieldPlaceholders_impl<0>(def);
}

template<typename ...Ts, typename ...Vs>
std::string unpackFieldPlaceholdersOpt(std::tuple<Ts...> def, std::tuple<Vs...> values)
{
    return details::unpackFieldPlaceholdersOpt_impl<0>(def, values);
}

template<typename ...Ts>
std::string unpackFieldsAndPlaceholders(Ts... def)
{
    return details::unpackFieldsAndPlaceholders_impl<0>(std::make_tuple(def...));
}

template<typename ...Ts>
std::string unpackFieldsAndPlaceholders(std::tuple<Ts...> def)
{
    return details::unpackFieldsAndPlaceholders_impl<0>(def);
}

template<typename ...Ts, typename ...Vs>
std::string unpackFieldsAndPlaceholdersOpt(std::tuple<Ts...> def, std::tuple<Vs...> values)
{
    return details::unpackFieldsAndPlaceholdersOpt_impl<0>(def, values);
}

template<typename ...Ts>
std::string unpackFieldDefinitions(std::tuple<Ts...> def)
{
    return details::unpackFieldDefinitions_impl<0>(def);
}

template<typename ...Ts>
std::string unpackFieldDefinitions(Ts... def)
{
    return details::unpackFieldDefinitions_impl<0>(std::make_tuple(def...));
}


class StatementFormatter {
public:
    virtual ~StatementFormatter() noexcept = default;

    virtual std::string string() const = 0;
};

class Select : public StatementFormatter {
    std::string mSelectBase;
    std::string mSelectOp;
    std::string mWhere;
    std::string mGroupBy;
public:
    Select() = default;

    template<typename ...F>
    explicit Select(std::string tablename, std::tuple<F...> flds)
    {
        mSelectOp = "SELECT ";
        std::ostringstream ss;
        ss << unpackFieldNames(flds) << " FROM " << tablename;

        mSelectBase = ss.str();
    }

    template<typename ...F>
    explicit Select(std::string tablename, F... flds)
            : Select(tablename, std::make_tuple(flds...))
    {
    }

    Select &distinct()
    {
        mSelectOp = "SELECT DISTINCT ";
        return *this;
    }

    template<typename ...F>
    Select &groupBy(F... flds)
    {
        mGroupBy = " GROUP BY " + unpackFieldNames(flds...);
        return *this;
    }

    Select &where(std::string condition)
    {
        if (condition.empty()) {
            mWhere = "";
        } else {
            mWhere = " WHERE " + condition;
        }
        return *this;
    }

    std::string string() const override
    {
        std::ostringstream ss;
        ss << mSelectOp << mSelectBase << mWhere << mGroupBy << "";
        return ss.str();
    }

    template<typename T>
    void join(const std::string &tableName, const FieldDef <T> fld1, FieldDef <T> fld2)
    {
        std::ostringstream ss;
        ss << mSelectBase << " JOIN " << tableName << " ON " << fld1.name() << " = " << fld2.name();
        mSelectBase = ss.str();
    }
};

class CreateTable : public StatementFormatter {
    std::string mName;
    std::string mStatementString;
    std::string mConstraintString;
public:
    CreateTable() = default;

    template<typename ...F>
    explicit CreateTable(std::string tablename, F... fields)
    {
        mName = tablename;
        std::ostringstream ss;
        ss << unpackFieldDefinitions(fields...);
        mStatementString = ss.str();
    }

    std::string string() const override
    {
        std::ostringstream ss;
        ss << "CREATE TABLE " << mName << " ("
           << mStatementString << mConstraintString << ")";
        return ss.str();
    }

    template<typename CONSTRAINT>
    void setConstraint(CONSTRAINT c)
    {
        mConstraintString = ", " + details::toString(c);
    }

    struct TableConstraint {
        class ForeignKey {
            std::string statement;
            std::string actionString;

        public:
            template<typename ...FLDS, typename ...REF_FLDS>
            explicit
            ForeignKey(std::string name, std::tuple<FLDS...> f, std::string refTable, std::tuple<REF_FLDS...> refFields)
            {
                std::ostringstream ss;
                ss << "CONSTRAINT " << name << " FOREIGN KEY("
                   << unpackFieldNames(f)
                   << ") REFERENCES " << refTable << "("
                   << unpackFieldNames(refFields)
                   << ")";
                statement = ss.str();
            }

            enum class Action {
                SetNull, SetDefault, Cascade, Restrict, NoAction
            };

        private:
            std::string translateActionString(Action a)
            {
                switch (a) {
                    case Action::SetNull:
                        return "SET NULL";
                    case Action::SetDefault:
                        return "SET DEFAULT";
                    case Action::Cascade:
                        return "CASCADE";
                    case Action::Restrict:
                        return "RESTRICT";
                    case Action::NoAction:
                        return "NO ACTION";
                }
                std::ostringstream ss;
                ss << "Not all switch case are handled in translateActionString: " << static_cast<int>(a);
                throw std::logic_error(ss.str());
            }

        public:
            void onDelete(Action a)
            {
                std::ostringstream ss;
                ss << actionString << " ON DELETE " << translateActionString(a);
                actionString = ss.str();
            }

            void onUpdate(Action a)
            {
                std::ostringstream ss;
                ss << actionString << " ON UPDATE " << translateActionString(a);
                actionString = ss.str();
            }

            std::string toString() const
            {
                std::ostringstream ss;
                ss << statement << actionString;
                return ss.str();
            }
        };

        class Unique {
            std::string statement;
        public:
            template<typename ...FLDS>
            explicit
            Unique(std::string keyname, FLDS... f)
            {
                std::ostringstream ss;
                ss << "CONSTRAINT " << keyname << " UNIQUE ("
                   << unpackFieldNames(std::make_tuple(f...))
                   << ")";
                statement = ss.str();
            }
            std::string toString() const
            {
                std::ostringstream ss;
                ss << statement;
                return ss.str();
            }
        };

        class PrimaryKey {
            std::string statement;
        public:
            template<typename ...FLDS>
            explicit
            PrimaryKey(std::string keyname, FLDS... f)
            {
                std::ostringstream ss;
                ss << "CONSTRAINT " << keyname << " PRIMARY KEY ("
                   << unpackFieldNames(std::make_tuple(f...))
                   << ")";
                statement = ss.str();
            }
            std::string toString() const
            {
                std::ostringstream ss;
                ss << statement;
                return ss.str();
            }
        };
    };
};

class CreateIndex : public StatementFormatter {
    std::string mAction;
    std::string mName;
    std::string mTableName;
    std::string mFieldsPack;
public:
    struct UniqueIndexType {};
    static constexpr UniqueIndexType Unique {};

    CreateIndex() = default;

    template<typename ...F>
    explicit CreateIndex(std::string indexname, std::string tablename, F... fields)
            : mAction("CREATE INDEX"),
              mName(std::move(indexname)),
              mTableName(std::move(tablename))
    {
        std::ostringstream ss;
        ss << unpackFieldNames(fields...);
        mFieldsPack = ss.str();
    }

    template<typename ...F>
    explicit CreateIndex(UniqueIndexType, std::string indexname, std::string tablename, F... fields)
            : mAction("CREATE UNIQUE INDEX"),
              mName(std::move(indexname)),
              mTableName(std::move(tablename))
    {
        std::ostringstream ss;
        ss << unpackFieldNames(fields...);
        mFieldsPack = ss.str();
    }

    std::string string() const override
    {
        std::ostringstream ss;
        ss << mAction << " " << mName << " ON " << mTableName << "("
           << mFieldsPack << ")";
        return ss.str();
    }};

class Insert : public StatementFormatter {
    std::string mAction;
    std::string mStatementString;
public:
    Insert() = default;

    template<typename ...F>
    explicit Insert(std::string tablename, F... flds)
    {
        mAction = "INSERT ";
        std::ostringstream ss;

        ss << "INTO " << tablename << "("
           << unpackFieldNames(flds...) << ") VALUES("
           << unpackFieldPlaceholders(flds...) << ")";


        mStatementString = ss.str();
    }

    void doReplace()
    {
        mAction = "INSERT OR REPLACE ";
    }

    std::string string() const override
    {
        std::ostringstream ss;
        ss << mAction << mStatementString;
        return ss.str();
    }
};

class Delete : public StatementFormatter {
    std::string mAction;
    std::string mWhere;
public:
    Delete() = default;

    explicit Delete(std::string tablename)
    {
        std::ostringstream ss;
        ss << "DELETE FROM " << tablename;
        mAction = ss.str();
    }

    Delete &where(std::string condition)
    {
        if (!condition.empty())
            mWhere = " WHERE " + condition;
        else
            mWhere = "";
        return *this;
    }

    std::string string() const override
    {
        std::ostringstream ss;
        ss << mAction << mWhere;
        return ss.str();
    }
};

class Update : public StatementFormatter {
    std::string mAction = "UPDATE ";
    std::string mDefinition;
    std::string mWhere;
public:
    Update() = default;

    template<typename ...F>
    explicit Update(std::string tablename, std::tuple<F...> flds)
    {
        std::ostringstream ss;

        ss << tablename << " SET "
           << unpackFieldsAndPlaceholders(flds);
        mDefinition = ss.str();
    }
    template<typename ...F, typename ...Vs>
    explicit Update(std::string tablename, std::tuple<F...> flds, std::tuple<Vs...> values)
    {
        std::ostringstream ss;

        ss << tablename << " SET "
           << unpackFieldsAndPlaceholdersOpt(flds, values);
        mDefinition = ss.str();
    }

    Update &where(std::string condition)
    {
        if (!condition.empty())
            mWhere = " WHERE " + condition;
        else
            mWhere = "";
        return *this;
    }

    std::string string() const override
    {
        std::ostringstream ss;
        ss << mAction
           << mDefinition
           << mWhere;
        return ss.str();
    }

    enum class OrAction {
        Rollback, Abort, Replace, Fail, Ignore
    };

    Update &orAction(OrAction o)
    {
        switch (o) {
            case OrAction::Rollback:
                mAction = "UPDATE OR ROLLBACK ";
                break;
            case OrAction::Abort:
                mAction = "UPDATE OR ABORT ";
                break;
            case OrAction::Replace:
                mAction = "UPDATE OR REPLACE ";
                break;
            case OrAction::Fail:
                mAction = "UPDATE OR FAIL ";
                break;
            case OrAction::Ignore:
                mAction = "UPDATE OR IGNORE ";
                break;
        }
        return *this;
    }
};

}
}
#endif //SQLITE_SQLITESTATEMENTFORMATTERS_H
