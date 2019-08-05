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
    CreateFlag mCreateFlag;
};

Storage::Storage(std::string path, OpenMode openMode, CreateFlag createFlag)
        : p(spimpl::make_impl<Impl>())
{
    p->dbPath = path;
    p->mCreateFlag = createFlag;
    if (openMode == OpenMode::ImmediateOpen) {
        open();
    }
}

Storage::~Storage() noexcept
{
    if (p->mDb != nullptr) {
        sqlite3_close(p->mDb);
    }
}

Storage Storage::inMemory(OpenMode openMode, CreateFlag createFlag)
{
    return Storage(":memory:", openMode, createFlag);
}

void Storage::open()
{
    if (p->mDb != nullptr) {
        // already open, return.
        return;
    }

    auto openFlags = SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE ;
    if (p->mCreateFlag == CreateFlag::Create) {
        openFlags |= SQLITE_OPEN_CREATE;
    }

    auto r = sqlite3_open_v2(p->dbPath.c_str(), &p->mDb,
                             openFlags,
                             nullptr);
    if (r != SQLITE_OK) {
        throw Exception(p->mDb);
    }
    sqlite3_busy_timeout(p->mDb, 1000);

    updateFlags();
}

void Storage::close()
{
    sqlite3_close(p->mDb);
    p->mDb = nullptr;
}

void Storage::updateFlags()
{
    int r;

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
}

sqlite3 *Storage::handle()
{
    return p->mDb;
}

void Storage::setFlag(Storage::Flags flag)
{
    p->mFlags.insert(flag);
    if (isOpen()) {
        updateFlags();
    }
}

bool Storage::isOpen()
{
    return p->mDb != nullptr;
}

bool Storage::execute(Statement &s)
{
    return s.execute(*this);
}

}
}