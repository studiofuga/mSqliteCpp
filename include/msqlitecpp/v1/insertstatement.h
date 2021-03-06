/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 14/03/18
 */

#ifndef SQLITE_INSERTSTATEMENT_H
#define SQLITE_INSERTSTATEMENT_H

#include "msqlitecpp/v1/sqlitestorage.h"
#include "msqlitecpp/v1/sqlitestatement.h"
#include "msqlitecpp/v1/sqlitestatementformatters.h"

#include <cstddef>

namespace sqlite {

template<typename ...FIELDS>
class InsertStatement {
    std::tuple<FIELDS...> fields;
    std::shared_ptr<SQLiteStorage> db;
    std::shared_ptr<SQLiteStatement> statement;
    std::string tablename;
    enum class Action {
        NoAction, Replace, Ignore
    };
    Action conflictAction = Action::NoAction;

    template <typename T>
    bool bindValue(int idx, const T& t) {
        sqlite::bind(*statement, idx+1, t);
        return true;
    }

#if defined(WITH_BOOST)
    template <typename T>
    bool bindValue(int idx, const boost::optional<T> & t) {
        if (t) {
            sqlite::bind(*statement, idx+1, t.value());
            return true;
        }
        sqlite::bind(*statement, idx+1, nullptr);
        return true;
    }
#endif

    bool bindValue(int idx, std::nullptr_t t) {
        sqlite::bind(*statement, idx+1, nullptr);
        return true;
    }

    template<int N = 0, typename ...T>
    typename std::enable_if<N == sizeof...(T), void>::type insertImpl(std::tuple<T...>, size_t idx = 0)
    {
    };

    template<int N = 0, typename ...T>
    typename std::enable_if<N < sizeof...(T), void>::type insertImpl(std::tuple<T...> values, size_t idx = 0)
    {
        bindValue(idx, std::get<N>(values));
                ++idx;
        insertImpl<N + 1>(values, idx);
    };

public:
    InsertStatement() = default;

    explicit InsertStatement(FIELDS... f)
    { fields = std::make_tuple(f...); }

    void replaceOnConflict()
    {
        if (statement) {
            throw std::logic_error("Cannot call InsertStatement::replaceOnConflict() after InsertStatement::Attach()");
        }
        conflictAction = Action::Replace;
    }

    void ignoreOnConflict()
    {
        if (statement) {
            throw std::logic_error("Cannot call InsertStatement::ignoreOnConflict() after InsertStatement::Attach()");
        }
        conflictAction = Action::Ignore;
    }

    void attach(std::shared_ptr<SQLiteStorage> dbm, std::string table)
    {
        db = dbm;
        tablename = std::move(table);
        prepare();
    }

    void prepare() {
        auto insertStatement = statements::Insert(tablename, fields);
        if (conflictAction == Action::Replace) {
            insertStatement.doReplace();
        } else if(conflictAction == Action::Ignore) {
            insertStatement.doIgnoreOnConflict();
        }
        statement = std::make_shared<SQLiteStatement>(db, insertStatement);
    }

    template<typename ...T>
    void insert(T... values)
    {
        insertImpl<0>(std::make_tuple(values...));
        statement->execute();
    }

};

template<typename ...FIELDS>
inline InsertStatement<FIELDS...> makeInsertStatement(FIELDS... fields)
{
    return InsertStatement<FIELDS...>(fields...);
}


} // ns

#endif //SQLITE_INSERTSTATEMENT_H
