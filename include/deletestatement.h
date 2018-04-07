//
// Created by Federico Fuga on 07/04/18.
//

#ifndef SQLITE_DELETESTATEMENT_H
#define SQLITE_DELETESTATEMENT_H

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>
#include "sqlitestorage.h"
#include "sqlitestatement.h"

namespace sqlite {

class DeleteStatement {
    std::shared_ptr<SQLiteStorage> db;
    std::string name;
    SQLiteStatement statement;

    statements::Delete sql;
public:
    DeleteStatement() = default;

    void attach (std::shared_ptr<SQLiteStorage> db, std::string name) {
        sql = statements::Delete(name);
        this->db = db;
        this->name = std::move(name);
    }

    void prepare() {
        std::cout << sql.string() << "\n";
        statement.attach(db, sql);
    }

    template <typename W>
    void where (W w) {
        sql.where(w.toText());
    }

    void exec() {
        statement.execute();
    }

    SQLiteStatement *getStatement() {
        return &statement;
    }
};

}

#endif //SQLITE_DELETESTATEMENT_H
