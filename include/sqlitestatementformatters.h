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


            /// @brief Ends the recursion of unpackFieldsAndPlaceholders_impl()
            template <size_t I, typename ...Ts>
            std::string unpackFieldsAndPlaceholders_impl (const std::tuple<Ts...> &,
                                                          typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
                return std::string();
            }
            /// @brief unpacks the fields into a list of strings in the form (FieldName = ?[,FieldName = ?...])
            template <size_t I, typename ...Ts>
            std::string unpackFieldsAndPlaceholders_impl (const std::tuple<Ts...> &def,
                                                          typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
                auto const &field = std::get<I>(def);
                return field.name()
                       + " = ?" + (I == sizeof...(Ts) - 1 ? "" : ",")
                       + unpackFieldsAndPlaceholders_impl<I+1>(def);
            }


        template <size_t I, typename ...Ts>
        std::string unpackFieldDefinitions_impl (const std::tuple<Ts...> &,
                                                 typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
            return std::string();
        }

        template <size_t I, typename ...Ts>
        std::string unpackFieldDefinitions_impl (const std::tuple<Ts...> &def,
                                                      typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
            auto const &field = std::get<I>(def);
            return field.name() + " " + field.sqlType() + field.sqlAttributes()
                   + (I == sizeof...(Ts) - 1 ? "" : ", ")
                   + unpackFieldDefinitions_impl<I+1>(def);
        }
        }

        template<typename ...Ts>
        std::string unpackFieldNames(Ts... def) {
            return details::unpackFieldNames_impl<0>(std::make_tuple(def...));
        }

        template<typename ...Ts>
        std::string unpackFieldNames(std::tuple<Ts...> def) {
            return details::unpackFieldNames_impl<0>(def);
        }


        template<typename ...Ts>
        std::string unpackFieldPlaceholders(Ts... def) {
            return details::unpackFieldPlaceholders_impl<0>(std::make_tuple(def...));
        }

        template<typename ...Ts>
        std::string unpackFieldPlaceholders(std::tuple<Ts...> def) {
            return details::unpackFieldPlaceholders_impl<0>(def);
        }

        template<typename ...Ts>
        std::string unpackFieldsAndPlaceholders(Ts... def) {
            return details::unpackFieldsAndPlaceholders_impl<0>(std::make_tuple(def...));
        }

        template<typename ...Ts>
        std::string unpackFieldsAndPlaceholders(std::tuple<Ts...> def) {
            return details::unpackFieldsAndPlaceholders_impl<0>(def);
        }

    template<typename ...Ts>
    std::string unpackFieldDefinitions(std::tuple<Ts...> def) {
        return details::unpackFieldDefinitions_impl<0>(def);
    }

    template<typename ...Ts>
    std::string unpackFieldDefinitions(Ts... def) {
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

            template <typename ...F>
            explicit Select(std::string tablename, std::tuple<F...> flds) {
                mSelectOp = "SELECT ";
                std::ostringstream ss;
                ss << unpackFieldNames(flds) << " FROM " << tablename;

                mSelectBase = ss.str();
            }
            template <typename ...F>
            explicit Select (std::string tablename, F... flds)
            : Select(tablename, std::make_tuple(flds...)) {
            }

            Select &distinct() {
                mSelectOp = "SELECT DISTINCT ";
                return *this;
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
                std::ostringstream ss;
                ss << mSelectOp << mSelectBase << mWhere << mGroupBy << ";";
                return ss.str();
            }

            template <typename T>
            void join(const std::string &tableName, const FieldDef<T> fld1, FieldDef<T> fld2) {
                std::ostringstream ss;
                ss << mSelectBase <<  " JOIN " << tableName << " ON " << fld1.name() << " = " << fld2.name();
                mSelectBase = ss.str();
            }
        };

        class Create : public StatementFormatter {
            std::string mStatementString;
        public:
            template <typename ...F>
            explicit Create(std::string tablename, F... fields) {
                std::ostringstream ss;

                ss << "CREATE TABLE " << tablename << " ("
                                                      << unpackFieldDefinitions(fields...) << ");";
                mStatementString = ss.str();
            }

            std::string string() const override {
                std::ostringstream ss;
                ss << mStatementString;
                return ss.str();
            }
        };

        class Insert : public StatementFormatter {
            std::string mAction;
            std::string mStatementString;
        public:
            template <typename ...F>
            explicit Insert (std::string tablename, F... flds) {
                mAction = "INSERT ";
                std::ostringstream ss;

                ss << "INTO " << tablename << "("
                                                  << unpackFieldNames(flds...) << ") VALUES("
                                                                            << unpackFieldPlaceholders(flds...) << ");";


                mStatementString = ss.str();
            }

            void doReplace() {
                mAction = "INSERT OR REPLACE ";
            }

            std::string string() const override {
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
            explicit Delete(std::string tablename) {
                std::ostringstream ss;
                ss << "DELETE FROM " << tablename;
                mAction = ss.str();
            }
            Delete &where(std::string condition) {
                mWhere = " WHERE " + condition;
                return *this;
            }

            std::string string() const override
            {
                std::ostringstream ss;
                ss << mAction << mWhere << ";";
                return ss.str();
            }
        };

        class Update : public StatementFormatter {
            std::string mAction = "UPDATE ";
            std::string mDefinition;
            std::string mWhere;
        public:
            Update() = default;
            template <typename ...F>
            explicit Update (std::string tablename, F... flds) {
                std::ostringstream ss;

                ss << tablename << " SET "
                   << unpackFieldsAndPlaceholders(flds...);
                mDefinition = ss.str();
            }

            Update &where(std::string condition) {
                mWhere = " WHERE " + condition;
                return *this;
            }

            std::string string() const override {
                std::ostringstream ss;
                ss << mAction
                   << mDefinition
                   << mWhere << ";";
                return ss.str();
            }

            enum class OrAction {
                Rollback, Abort, Replace, Fail, Ignore
            };

            Update &orAction(OrAction o) {
                switch (o) {
                    case OrAction ::Rollback:
                        mAction = "UPDATE OR ROLLBACK "; break;
                    case OrAction ::Abort:
                        mAction = "UPDATE OR ABORT "; break;
                    case OrAction ::Replace:
                        mAction = "UPDATE OR REPLACE "; break;
                    case OrAction ::Fail:
                        mAction = "UPDATE OR FAIL "; break;
                    case OrAction ::Ignore:
                        mAction = "UPDATE OR IGNORE "; break;
                }
                return *this;
            }
        };

    }
}
#endif //SQLITE_SQLITESTATEMENTFORMATTERS_H
