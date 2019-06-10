/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_FORMATTERS_H
#define MSQLITECPP_FORMATTERS_H

#include <string>
#include <tuple>
#include <sstream>


namespace msqlitecpp {
namespace v2 {

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
inline std::string toString(msqlitecpp::v2::Column<Q> t)
{
    return t.name();
}

/*
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
*/

template<typename T>
std::string formatFieldName(const T &t, const std::string &sep)
{
    return sep + toString(t);
}

template<typename F, typename T>
std::string formatFieldName(const F &f, const T &t, const std::string &sep)
{
    return sep + toString(f);
}

/*
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
*/

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
    if (!fld.empty()) {
        ++count;
    }
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
    if (!fld.empty()) {
        ++count;
    }
    return fld + unpackFieldNamesOpt_impl<I + 1, Ts...>(def, v, count);
}

template<typename T>
std::string formatFieldPlaceholder3(const T &t, const std::string &sep, size_t I = 0)
{
    std::ostringstream ss;
    ss << sep << "?" << I + 1;
    return ss.str();
}

template<typename F, typename T>
std::string formatFieldPlaceholder4(const F &f, const T &t, const std::string &sep, size_t I = 0)
{
    std::ostringstream ss;
    ss << sep << "?" << I + 1;
    return ss.str();
}

/*
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
*/

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
    auto fld = formatFieldPlaceholder3(std::get<I>(def), (count > 0 ? "," : ""), I);
    if (!fld.empty()) {
        ++count;
    }
    return fld + unpackFieldPlaceholders_impl<I + 1, Ts...>(def, count);
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
    auto fld = formatFieldPlaceholder4(std::get<I>(def), std::get<I>(values), (count > 0 ? "," : ""), I);
    if (!fld.empty()) {
        ++count;
    }
    return fld + unpackFieldPlaceholdersOpt_impl<I + 1, Ts...>(def, values, count);
}


template<typename T>
std::string formatFieldNameAndPlaceholder(const T &t, const std::string &sep, size_t I = 0)
{
    std::ostringstream ss;
    ss << sep << toString(t) << " = ?" << I + 1;
    return ss.str();
}

/*
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

*/
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
    auto fld = formatFieldNameAndPlaceholder(std::get<I>(def), (count > 0 ? "," : ""), I);
    if (!fld.empty()) {
        ++count;
    }

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
    auto fld = formatFieldNameAndPlaceholderOpt(std::get<I>(def), std::get<I>(values), (count > 0 ? "," : ""), I);
    if (!fld.empty()) {
        ++count;
    }

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


}
}

#endif //MSQLITECPP_FORMATTERS_H
