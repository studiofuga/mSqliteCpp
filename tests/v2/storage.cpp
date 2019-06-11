/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

TEST(Storage, createInMemory)
{
    auto storage = Storage::inMemory();

    EXPECT_TRUE(storage.isOpen());
    ASSERT_NO_THROW(storage.close());
}

TEST(Storage, createWithFlags)
{
    auto storage = Storage::inMemory(Storage::OpenMode::DelayedOpen);

    EXPECT_FALSE(storage.isOpen());
    ASSERT_NO_THROW(storage.open());

    EXPECT_TRUE(storage.isOpen());
    ASSERT_NO_THROW(storage.close());
}
