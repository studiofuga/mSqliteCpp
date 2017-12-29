//
// Created by Federico Fuga on 29/12/17.
//

#include <gtest/gtest.h>

#include "sqlitefielddef.h"
#include "sqlitestatementformatters.h"

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

    ASSERT_EQ(sqlite::statements::Select("Table", fldId, fldName).groupBy(fldValue).string(),
              "SELECT id,name FROM Table GROUP BY value;"
    );
}

