/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/statement.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

TEST(Statements, executeString)
{
    auto str = "CREATE TABLE t (i INTEGER, t TEXT, r REAL, b BLOB);";

    auto db = Storage::inMemory();

    auto statement = Statement(str);

    ASSERT_TRUE(statement.execute(db));

    auto str2 = "CREATE TABLE t2 (i INTEGER, t TEXT, r REAL, b BLOB);";

    ASSERT_NO_THROW(statement.set(str2));

    ASSERT_TRUE(statement.execute(db));

    ASSERT_NO_THROW(db.close());
}

TEST(Statements, executeWithBind)
{
    auto crstr = "CREATE TABLE t (i INTEGER, t TEXT, r REAL, b BLOB);";

    auto db = Storage::inMemory();

    ASSERT_NO_THROW(db.open());

    auto create_statement = Statement(db, crstr);

    ASSERT_TRUE(create_statement.execute(db));

    auto str = "INSERT INTO t(i,r,t) VALUES (?,?,?);";

    auto statement = Statement(db, str);

    ASSERT_NO_THROW(statement.bind(1, 100));
    ASSERT_NO_THROW(statement.bind(2, 1.5));
    ASSERT_NO_THROW(statement.bind(3, "sample"));
    ASSERT_NO_THROW(statement.execute(db));

    ASSERT_NO_THROW(statement.bind(std::make_tuple(200, 1.3, "anothersample")));
    ASSERT_NO_THROW(statement.execute(db));
}
