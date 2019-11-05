//
// Created by happycactus on 11/5/19.
//

#include "msqlitecpp/version.h"

namespace msqlitecpp {

uint32_t version()
{
    return (MSQLITECPP_VERSION_MAJOR << 24) |
           (MSQLITECPP_VERSION_MINOR << 16) |
           (MSQLITECPP_VERSION_PATCH << 8) |
           (MSQLITECPP_VERSION_TWEAK);

}

std::string version_string()
{
    return std::string(MSQLITECPP_VERSION);
}

}