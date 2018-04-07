/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 14/03/18
 */

#ifndef SQLITE_INSERTSTATEMENT_H
#define SQLITE_INSERTSTATEMENT_H

#include "sqlitestorage.h"
#include "sqlitestatement.h"
#include "sqlitestatementformatters.h"

namespace sqlite {

template<typename ...FIELDS>
class InsertStatement {
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    std::shared_ptr<SQLiteStatement> statement;
    std::string tablename;
    bool do_replace = false;

    template <int N=0, typename ...T>
    typename std::enable_if<N == sizeof...(T), void>::type insertImpl(std::tuple<T...>) {
    };

    template <int N=0, typename ...T>
    typename std::enable_if<N < sizeof...(T), void>::type insertImpl(std::tuple<T...> values) {
        statement->bind(N+1, std::get<N>(values));
        insertImpl<N+1>(values);
    };
public:
    InsertStatement() {}
    explicit InsertStatement(FIELDS... f) { fields = std::make_tuple(f...); }

    void doReplace() {
        if (statement)
            throw std::logic_error("Cannot call InsertStatement::doReplace() after InsertStatement::Attach()");
        do_replace = true;
    }

    void attach (std::shared_ptr<SQLiteStorage> dbm, std::string table) {
        db = dbm;
        tablename = std::move(table);
        auto r = statements::Insert(tablename, fields);
        if (do_replace)
            r.doReplace();
        statement = std::make_shared<SQLiteStatement>(db, r);
    }

    template <typename ...T>
    void insert (T... values) {
        insertImpl<0>(std::make_tuple(values...));
        statement->execute();
    }

};

template <typename ...FIELDS>
inline InsertStatement<FIELDS...> makeInsertStatement(FIELDS... fields) {
    return InsertStatement<FIELDS...>(fields...);
}


} // ns

#endif //SQLITE_INSERTSTATEMENT_H