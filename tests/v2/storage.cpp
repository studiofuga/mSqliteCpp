/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

TEST(Storage, createInMemory)
{
    auto storage = Storage::makeInMemory();

    ASSERT_NO_THROW(storage.open());
    ASSERT_NO_THROW(storage.close());
}
