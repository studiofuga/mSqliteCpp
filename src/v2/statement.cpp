/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/statement.h"
#include "msqlitecpp/v2/exceptions.h"

#include <sqlite3.h>

namespace msqlitecpp {
namespace v2 {

struct Statement::Impl {
    std::string sqlStatement;

    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr;

    explicit Impl(std::string sql)
            : sqlStatement(std::move(sql))
    {

    }

    explicit Impl(Storage &s)
            : db(s.handle())
    {

    }

    explicit Impl(Storage &s, std::string sql)
            : db(s.handle()), sqlStatement(std::move(sql))
    {

    }

    void prepareIfNecessary()
    {
        if (stmt == nullptr) {
            auto r = sqlite3_prepare_v2(db, sqlStatement.c_str(), -1, &stmt, nullptr);
            if (r != SQLITE_OK) {
                throw Exception(db, sqlStatement);
            }
        }
    }

    void prepare(sqlite3 *dbp = nullptr)
    {
        if (dbp != nullptr) {
            db = dbp;
        }

        auto r = sqlite3_prepare_v2(db, sqlStatement.c_str(), -1, &stmt, nullptr);
        if (r != SQLITE_OK) {
            throw Exception(db, sqlStatement);
        }
    }
};

Statement::Statement(Storage &storage)
        : p(spimpl::make_impl<Impl>(storage))
{

}

Statement::Statement(char const *sql)
        : p(spimpl::make_impl<Impl>(sql))
{

}

Statement::Statement(Storage &db, char const *sql)
        : p(spimpl::make_impl<Impl>(db, sql))
{

}

Statement::~Statement()
{
    if (p != nullptr && p->stmt != nullptr) {
        sqlite3_finalize(p->stmt);
    }
}

void Statement::bind(size_t idx, std::string value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_text(p->stmt, idx, value.c_str(), value.length(), SQLITE_TRANSIENT);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, unsigned long long value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, long long value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, unsigned long value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, long value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_int64(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, unsigned int value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_int(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, int value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_int(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, double value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_double(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, float value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_double(p->stmt, idx, value);
    Exception::throwIfNotOk(r, p->db);
}

void Statement::bind(size_t idx, std::nullptr_t value)
{
    p->prepareIfNecessary();
    auto r = sqlite3_bind_null(p->stmt, idx);
    Exception::throwIfNotOk(r, p->db);
}


Statement::QueryResult Statement::executeStep(std::function<bool()> func)
{
    auto r = sqlite3_step(p->stmt);
    if (r == SQLITE_DONE) {
        return QueryResult::Completed;
    } else if (r != SQLITE_ROW) {
        Exception::throwIfNotOk(r, p->db);
    }

    return (func() ? QueryResult::Ongoing : QueryResult::Aborted);
}

Statement::QueryResult Statement::executeStep()
{
    return executeStep([]() { return true; });
}

bool Statement::execute(std::function<bool()> function)
{
    QueryResult result;
    try {
        while ((result = executeStep(function)) == Statement::QueryResult::Ongoing) {}
    } catch (Exception &) {
        sqlite3_reset(p->stmt); // Reset the statement before throwing again
        throw;
    }
    sqlite3_reset(p->stmt);
    sqlite3_clear_bindings(p->stmt);
    return result == Statement::QueryResult::Completed;
}

bool Statement::execute()
{
    return execute([]() { return true; });
}

bool Statement::execute(Storage &db)
{
    p->prepare(db.handle());
    return execute();
}

}
}