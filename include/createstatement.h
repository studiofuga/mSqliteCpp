/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 08/05/18
 */

#ifndef SQLITE_CREATESTATEMENT_H
#define SQLITE_CREATESTATEMENT_H

#include "sqlitestorage.h"
#include "sqlitestatement.h"

#include <type_traits>
#include <functional>
#include <tuple>
#include <utility>
#include <iostream>

namespace sqlite {

template<typename ...FIELDS>
class CreateStatement {
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    std::shared_ptr<SQLiteStatement> statement;
    std::string tablename;
public:
    CreateStatement() = default;
    explicit CreateStatement(FIELDS... f) {
        fields = std::make_tuple(f...);
    }
    CreateStatement(std::shared_ptr<SQLiteStorage> dbm, std::string table, FIELDS... f) {
        fields = std::make_tuple(f...);
        attach(dbm, table);
    }

    void attach (std::shared_ptr<SQLiteStorage> dbm, std::string table) {
        db = dbm;
        tablename = std::move(table);
        auto r = statements::Create(tablename, fields);
        statement = std::make_shared<SQLiteStatement>(db, r);
    }

    void execute() {
        statement->execute();
    }
};
}

#endif //SQLITE_CREATESTATEMENT_H
