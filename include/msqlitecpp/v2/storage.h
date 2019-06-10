/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#ifndef MSQLITECPP_STORAGE_H
#define MSQLITECPP_STORAGE_H

#include "msqlitecpp/utils/spimpl.h"
#include "msqlitecpp/v2/msqlitecpp.h"

#include <memory>

struct sqlite3;

namespace msqlitecpp {
namespace v2 {

class EXPORT_MSQLITEV2 Storage {
public:
    enum class Flags {
        EnforceForeignKeys
    };

private:
    struct Impl;
    spimpl::impl_ptr<Impl> p;
public:
    explicit Storage(std::string path);

    static Storage makeInMemory();

    ~Storage() noexcept;

    void setFlag(Flags);

    bool open();

    bool close();

    sqlite3 *handle();

};

}
}

#endif //MSQLITECPP_STORAGE_H
