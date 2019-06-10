/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/06/19
 */

#include "msqlitecpp/v2/fields.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

TEST(Fields, definition)
{
    Column<ColumnTypes::Integer> i("typeI");

    EXPECT_EQ(i.name(), "typeI");
    EXPECT_EQ(i.sqlType(), "INTEGER");
}