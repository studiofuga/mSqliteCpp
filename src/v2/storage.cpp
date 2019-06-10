/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/exceptions.h"
#include "msqlitecpp/v2/statement.h"

#include <sqlite3.h>
#include <set>

namespace msqlitecpp {
namespace v2 {

struct Storage::Impl {
    std::string dbPath;

    sqlite3 *mDb = nullptr;

    std::set<Flags> mFlags;
};

Storage::Storage(std::string path)
        : p(spimpl::make_impl<Impl>())
{

}

Storage::~Storage() noexcept
{
    if (p->mDb != nullptr) {
        sqlite3_close(p->mDb);
    }
}

Storage Storage::inMemory()
{
    return Storage(":memory:");
}

bool Storage::open()
{
    auto r = sqlite3_open_v2(p->dbPath.c_str(), &p->mDb,
                             SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                             nullptr);
    if (r != SQLITE_OK) {
        throw Exception(p->mDb);
    }
    sqlite3_busy_timeout(p->mDb, 1000);

    for (auto &flag : p->mFlags) {
        switch (flag) {
            case Flags::EnforceForeignKeys:
                r = sqlite3_exec(p->mDb, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
                if (r != SQLITE_OK) {
                    throw Exception(p->mDb);
                }
                break;
            default:
                throw std::logic_error("Unhandled case for flag");
        }
    }

    return true;
}

bool Storage::close()
{
    sqlite3_close(p->mDb);
    p->mDb = nullptr;
    return true;
}

sqlite3 *Storage::handle()
{
    return p->mDb;
}

void Storage::setFlag(Storage::Flags flag)
{
    p->mFlags.insert(flag);
}

bool Storage::execute(Statement &s)
{
    return s.execute(*this);
}

}
}