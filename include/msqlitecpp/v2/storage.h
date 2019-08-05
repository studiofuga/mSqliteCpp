/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_STORAGE_H
#define MSQLITECPP_STORAGE_H

#include "msqlitecpp/utils/spimpl.h"
#include "msqlitecpp/v2/msqlitecpp.h"

#include <memory>
#include <string>

struct sqlite3;

namespace msqlitecpp {
namespace v2 {

class Statement;

class EXPORT_MSQLITEV2 Storage {
public:
    enum class Flags {
        EnforceForeignKeys
    };

    enum class CreateFlag {
        Create, DoNotCreate
    };

    enum class OpenMode {
        ImmediateOpen, DelayedOpen
    };

private:
    struct Impl;
    spimpl::impl_ptr<Impl> p;

    void updateFlags();
public:
    explicit Storage(std::string path, OpenMode openMode = OpenMode::ImmediateOpen, CreateFlag createFlag = CreateFlag::Create);

    Storage (Storage const &) = delete;
    Storage (Storage &&) = default;

    Storage & operator = (Storage const &) = delete;
    Storage & operator = (Storage &&) = default;

    static Storage inMemory(OpenMode openMode = OpenMode::ImmediateOpen, CreateFlag createFlag = CreateFlag::Create);

    ~Storage() noexcept;

    void setFlag(Flags);

    void open();

    void close();

    bool isOpen();

    sqlite3 *handle();

    bool execute(Statement &s);
};

}
}

#endif //MSQLITECPP_STORAGE_H
