//
// Created by Federico Fuga on 16/12/17.
//

#ifndef SQLITE_SQLITESTATEMENT_H
#define SQLITE_SQLITESTATEMENT_H

#include <sqlitestorage.h>

#include <memory>
#include <string>

#include <sqlite3.h>
#include "sqlitefielddef.h"
#include "sqlitestatementformatters.h"

#include <iostream>

namespace sqlite {

    class SQLiteStatement {
        struct Impl;

        std::unique_ptr<Impl> p;

        void init(std::shared_ptr<SQLiteStorage> db);
        void prepare (std::string sql);

        template <size_t I, typename ...Ts>
        void bind_impl(std::tuple<Ts...>, typename std::enable_if<I == sizeof...(Ts)>::type * = 0) {
        }

        template <size_t I, typename ...Ts>
        void bind_impl(std::tuple<Ts...> t, typename std::enable_if<I < sizeof...(Ts)>::type * = 0) {
            bind(I+1, std::get<I>(t));  // bind are 1-based, index are 0 based
            std::cout << "Bind: " << I+1 << " => " << std::get<I>(t) << "\n";
            bind_impl<I+1>(t);
        };
    public:
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> dbm, std::string stmt);
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> dbm, const char *stmt);
        explicit SQLiteStatement(std::shared_ptr<SQLiteStorage> db, const sqlite::statements::StatementFormatter &stmt);

        ~SQLiteStatement();

        void bind(size_t idx, std::string value);
        void bind(size_t idx, int value);
        void bind(size_t idx, double value);

        template <typename ...Ts>
        void bind(std::tuple<Ts...> t) {
            bind_impl<0>(t);
        }

        int getIntValue(int idx);
        double getDoubleValue(int idx);
        std::string getStringValue(int idx);

        FieldType::Type columnType(int idx);
        int columnCount();

        /** @brief Execute a step of a statement, calling a result manipulation function.
         *
         * @args function a functor that treats the results, returns true if ok, false if execution is completed
         * even before the sqlite3 engine has completed the statement.
         * @return true if ok and more data has to come, false if statement execution is completed
         * @throws SqliteException if error occurs
         */
        bool execute(std::function<bool()> function);
        bool execute();

        bool executeLoop(std::function<bool()> function);
        bool executeLoop();
    };

} // ns sqlite

#endif //SQLITE_SQLITESTATEMENT_H
