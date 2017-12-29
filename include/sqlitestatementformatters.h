//
// Created by Federico Fuga on 29/12/17.
//

#ifndef SQLITE_SQLITESTATEMENTFORMATTERS_H
#define SQLITE_SQLITESTATEMENTFORMATTERS_H

#include <sstream>

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
        }

        template<typename ...Ts>
        std::string unpackFieldNames(Ts... def) {
            return details::unpackFieldNames_impl<0>(std::make_tuple(def...));
        }

        class Select {
            std::string mString;
        public:
            template <typename ...F>
            explicit Select (std::string tablename, F... flds) {
                std::ostringstream ss;
                ss << "SELECT " << unpackFieldNames(flds...) << " FROM " << tablename << ";";

                mString = ss.str();
            }

            std::string string() const {
                return mString;
            }
        };

    }
}
#endif //SQLITE_SQLITESTATEMENTFORMATTERS_H
