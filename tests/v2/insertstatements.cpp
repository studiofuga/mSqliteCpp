/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/createstatement.h"
#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/insertstatement.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

TEST(InsertStatement, simple)
{
    auto db = Storage::inMemory();

    Column<ColumnTypes::Integer> colI("i");
    Column<ColumnTypes::Text> colT("t");
    Column<ColumnTypes::Real> colR("r");

    auto statement = makeCreateStatement(db, "t", colI, colT, colR);

    ASSERT_NO_THROW(statement.execute());

    auto insertStatement = makeInsertStatement(db, "t", colI, colT, colR);

    ASSERT_NO_THROW(insertStatement.insert(1, "one", 1.0));
    ASSERT_NO_THROW(insertStatement.insert(2, "two", 2.0));
    ASSERT_NO_THROW(insertStatement.insert(3, "three", 3.0));
}