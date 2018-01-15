//
// Created by Federico Fuga on 29/12/17.
//

#ifndef SQLITE_SQLITESTATEMENTFORMATTERS_H
#define SQLITE_SQLITESTATEMENTFORMATTERS_H

#include "sqlitefielddef.h"
#include <sstream>
#include <tuple>

namespace sqlite {
    namespace statements {

        namespace details {

            template<size_t I, typename ...Ts>
            std::string
            unpackFieldNames_impl(const std::tuple<Ts...> &def,
                                  typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
                (void) def;
                return std::string();
            }

            /// @brief builds a Field list part of a SQL Insert statement
            template<size_t I, typename ...Ts>
            std::string
            unpackFieldNames_impl(const std::tuple<Ts...> &def,
                                  typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
                auto &field = std::get<I>(def);
                return field.name() + (I == sizeof...(Ts) - 1 ? "" : ",") + unpackFieldNames_impl<I + 1, Ts...>(def);
            }

            template<size_t I, typename ...Ts>
            std::string
            unpackFieldPlaceholders_impl(const std::tuple<Ts...> &def,
                                  typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
                (void) def;
                return std::string();
            }

            /// @brief builds a Field list part of a SQL Insert statement
            template<size_t I, typename ...Ts>
            std::string
            unpackFieldPlaceholders_impl(const std::tuple<Ts...> &def,
                                  typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
                auto &field = std::get<I>(def);
                return std::string {"?"} + (I == sizeof...(Ts) - 1 ? "" : ",") + unpackFieldPlaceholders_impl<I + 1, Ts...>(def);
            }

        }

        template<typename ...Ts>
        std::string unpackFieldNames(Ts... def) {
            return details::unpackFieldNames_impl<0>(std::make_tuple(def...));
        }

        template<typename ...Ts>
        std::string unpackFieldPlaceholders(Ts... def) {
            return details::unpackFieldPlaceholders_impl<0>(std::make_tuple(def...));
        }

        class StatementFormatter {
        public:
            virtual ~StatementFormatter() noexcept = default;

            virtual std::string string() const = 0;
        };

        class Select : public StatementFormatter {
            std::string mSelectBase;
            std::string mWhere;
            std::string mGroupBy;
        public:
            template <typename ...F>
            explicit Select (std::string tablename, F... flds) {
                std::ostringstream ss;
                ss << "SELECT " << unpackFieldNames(flds...) << " FROM " << tablename;

                mSelectBase = ss.str();
            }

            template <typename ...F>
            Select &groupBy(F... flds) {
                mGroupBy = " GROUP BY " + unpackFieldNames(flds...);
                return *this;
            }

            Select &where(std::string condition) {
                mWhere = " WHERE " + condition;
                return *this;
            }

            std::string string() const override {
                return mSelectBase + mWhere + mGroupBy + ";";
            }

            template <typename T>
            void join(const std::string &tableName, const FieldDef<T> fld1, FieldDef<T> fld2) {
                std::ostringstream ss;
                ss << mSelectBase <<  " JOIN " << tableName << " ON " << fld1.name() << " = " << fld2.name();
                mSelectBase = ss.str();
            }
        };

        class Insert : public StatementFormatter {
            std::string mStatementString;
        public:
            template <typename ...F>
            explicit Insert (std::string tablename, F... flds) {
                std::ostringstream ss;

                ss << "INSERT INTO " << tablename << "("
                                                  << unpackFieldNames(flds...) << ") VALUES("
                                                                            << unpackFieldPlaceholders(flds...) << ");";


                mStatementString = ss.str();
            }

            std::string string() const override {
                return mStatementString;
            }
        };

    }
}
#endif //SQLITE_SQLITESTATEMENTFORMATTERS_H
