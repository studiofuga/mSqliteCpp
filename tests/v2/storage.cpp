/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/exceptions.h"

#include <boost/filesystem.hpp>

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

TEST(Storage, createFile)
{
    static const char *filename = "/tmp/temporaryNonExistentSqliteDb.db";
    boost::filesystem::path path(filename);

    boost::filesystem::remove(path);
    Storage storage(filename);

    ASSERT_TRUE(boost::filesystem::exists(path));
    storage.close();

    boost::filesystem::remove(path);
}

TEST(Storage, openNoCreateInMemory)
{
    static const char *filename = "/tmp/temporaryNonExistentSqliteDb.db";
    boost::filesystem::path path(filename);

    boost::filesystem::remove(path);
    Storage storage(filename, Storage::OpenMode::DelayedOpen, Storage::CreateFlag::DoNotCreate);

    EXPECT_THROW(storage.open(), msqlitecpp::v2::Exception);
    ASSERT_NO_THROW(storage.close());
}

