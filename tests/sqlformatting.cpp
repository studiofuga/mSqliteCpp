//
// Created by Federico Fuga on 29/12/17.
//

#include <gtest/gtest.h>

#include "sqlitefielddef.h"
#include "sqlitefieldsop.h"
#include "sqlitestatementformatters.h"

TEST(SqlFormatting, fieldOperators)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());

    auto sumId = sqlite::op::sum(fldId);

    ASSERT_EQ(sumId.name(), "SUM(id)");
    ASSERT_EQ(sqlite::op::avg(fldId).name(), "AVG(id)");
    ASSERT_EQ(sqlite::op::min(fldId).name(), "MIN(id)");
    ASSERT_EQ(sqlite::op::max(fldId).name(), "MAX(id)");
}

TEST(SqlFormatting, relOperators)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Integer());

    ASSERT_EQ(sqlite::op::eq(fldId), "id = ?");
    ASSERT_EQ(sqlite::op::lt(fldId), "id < ?");
    ASSERT_EQ(sqlite::op::and_(sqlite::op::lt(fldId),sqlite::op::gt(fldValue)), "id < ? AND value > ?");
    ASSERT_EQ(sqlite::op::or_(sqlite::op::lt(fldId),sqlite::op::gt(fldId)), "id < ? OR id > ?");
}

TEST(SqlFormatting, internalUnpack)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    std::string str = sqlite::statements::unpackFieldNames(fldId, fldName, fldValue);
    ASSERT_EQ(str, "id,name,value");
}

TEST(SqlFormatting, select)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    sqlite::statements::Select select("Table", fldId, fldName, fldValue);

    ASSERT_EQ(select.string(), "SELECT id,name,value FROM Table;");

    ASSERT_EQ(sqlite::statements::Select("Table", fldId, fldName).where(sqlite::op::eq(fldId)).string(),
              "SELECT id,name FROM Table WHERE id = ?;"
    );

    ASSERT_EQ(sqlite::statements::Select("Table", fldId, fldName).groupBy(fldValue).string(),
              "SELECT id,name FROM Table GROUP BY value;"
    );

    ASSERT_EQ(sqlite::statements::Select("Table", fldName,sqlite::op::sum(fldValue)).string(),
              "SELECT name,SUM(value) FROM Table;");
}

