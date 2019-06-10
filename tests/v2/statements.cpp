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
    auto str = "CREATE TABLE t (id INTEGER, name TEXT);";

    auto db = Storage::inMemory();

    ASSERT_NO_THROW(db.open());

    auto statement = Statement::make(str);

    ASSERT_TRUE(statement.execute(db));

    ASSERT_NO_THROW(db.close());
}
