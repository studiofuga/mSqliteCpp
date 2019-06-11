/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/createstatement.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

TEST(CreateStatement, create)
{
    auto db = Storage::inMemory();

    Column<ColumnTypes::Integer> colI("i");
    Column<ColumnTypes::Text> colT("t");
    Column<ColumnTypes::Real> colR("r");

    auto createStatement = makeCreateStatement(db, "t", colI, colT, colR);

    ASSERT_NO_THROW(createStatement.execute());
}