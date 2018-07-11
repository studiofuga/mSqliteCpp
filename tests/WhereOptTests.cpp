/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/07/18
 */

#include "clauses.h"
#include "operators.h"

#include <gtest/gtest.h>

using namespace sqlite;

TEST(WhereOptTests, formatOptionalOperatorsEq)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    EXPECT_EQ(operators::format(0, operators::eq(fldId),fieldSet), "id = ?1");
    EXPECT_EQ(operators::format(0, operators::eq(fldId),fieldNotSet), "");
}

TEST(WhereOptTests, formatOptionalOperatorsAnd)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldId2 = sqlite::makeFieldDef("id2", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto opAnd = operators::and_ (operators::eq(fldId), operators::eq(fldId2));
    EXPECT_EQ(opAnd.format(0, fieldSet, fieldSet), "id = ?1 AND id2 = ?2");
    EXPECT_EQ(opAnd.format(0, fieldSet, fieldNotSet), "id = ?1");
    EXPECT_EQ(opAnd.format(0, fieldNotSet, fieldSet), "id2 = ?2");
    EXPECT_EQ(opAnd.format(0, fieldNotSet, fieldNotSet), "");
}

TEST(WhereOptTests, formatWhereCase1)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto w = makeWhereOpt(operators::eq(fldId));

    EXPECT_EQ(w.format(fieldSet), "WHERE id = ?1");
    EXPECT_EQ(w.format(fieldNotSet), "WHERE TRUE");
}

TEST(WhereOptTests, formatWhereCase2)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldId2 = sqlite::makeFieldDef("id2", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId), operators::eq(fldId2)));

    EXPECT_EQ(w.format(fieldSet, fieldSet), "WHERE id = ?1 AND id2 = ?2");
    EXPECT_EQ(w.format(fieldSet, fieldNotSet), "WHERE id = ?1");
    EXPECT_EQ(w.format(fieldNotSet, fieldSet), "WHERE id2 = ?2");
    EXPECT_EQ(w.format(fieldNotSet, fieldNotSet), "WHERE TRUE");
}

