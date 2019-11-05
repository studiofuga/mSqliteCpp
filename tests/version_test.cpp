//
// Created by happycactus on 11/5/19.
//

#include "msqlitecpp/version.h"

#include <gtest/gtest.h>

TEST(Version, VersionTest)
{
    auto v = msqlitecpp::version();

    uint32_t myv =
            (MSQLITECPP_VERSION_MAJOR << 24) |
                    (MSQLITECPP_VERSION_MINOR << 16) |
                    (MSQLITECPP_VERSION_PATCH << 8)|
                    (MSQLITECPP_VERSION_TWEAK);

    ASSERT_EQ(v, myv);
    ASSERT_EQ(MSQLITECPP_VERSION, msqlitecpp::version_string());
}