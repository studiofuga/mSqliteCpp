//
// Created by Federico Fuga on 29/12/17.
//

#include <gtest/gtest.h>
#include <sqlitetable.h>

#include "sqlitefielddef.h"
#include "sqlitefieldsop.h"
#include "sqlitestatementformatters.h"
#include "sqlitestatement.h"

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

    std::string fields = sqlite::statements::unpackFieldNames(fldId, fldName, fldValue);
    ASSERT_EQ(fields, "id,name,value");
    fields = sqlite::statements::unpackFieldNames(fldId);
    ASSERT_EQ(fields, "id");

    auto placeholders = sqlite::statements::unpackFieldPlaceholders(fldId, fldName, fldValue);
    ASSERT_EQ(placeholders, "?,?,?");
    placeholders = sqlite::statements::unpackFieldPlaceholders(fldId);
    ASSERT_EQ(placeholders, "?");
}

TEST(SqlFormatting, select)
{
    auto db = std::make_shared<sqlite::SQLiteStorage>(":memory:");
    db->open();

    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    sqlite::SQLiteTable table(db, "tbl");
    table.create(std::make_tuple(fldId, fldName, fldValue));

    {
        sqlite::statements::Select select("tbl", fldId, fldName, fldValue);
        ASSERT_EQ(select.string(), "SELECT id,name,value FROM tbl;");
        ASSERT_NO_THROW(sqlite::SQLiteStatement(db, select));
    }

    {
        auto select = sqlite::statements::Select("tbl", fldId, fldName).where(sqlite::op::eq(fldId));
        ASSERT_EQ(select.string(), "SELECT id,name FROM tbl WHERE id = ?;");
        ASSERT_NO_THROW(sqlite::SQLiteStatement(db, select));
    }

    {
        auto select = sqlite::statements::Select("tbl", fldId, fldName).groupBy(fldValue);
        ASSERT_EQ(select.string(), "SELECT id,name FROM tbl GROUP BY value;");
        ASSERT_NO_THROW(sqlite::SQLiteStatement(db, select));
    }

    {
        auto select = sqlite::statements::Select("tbl", fldName, sqlite::op::sum(fldValue));
        ASSERT_EQ(select.string(),"SELECT name,SUM(value) FROM tbl;");
        ASSERT_NO_THROW(sqlite::SQLiteStatement(db, select));
    }

    {
        auto select = sqlite::statements::Select("tbl", fldName).distinct();
        ASSERT_EQ(select.string(),"SELECT DISTINCT name FROM tbl;");
        ASSERT_NO_THROW(sqlite::SQLiteStatement(db, select));
    }
}

TEST(SqlFormatting, insert)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    auto i = sqlite::statements::Insert("Table", fldId, fldName, fldValue);

    ASSERT_EQ(i.string(), "INSERT INTO Table(id,name,value) VALUES(?,?,?);");
}

TEST(SqlFormatting, deleteStatement)
{
    sqlite::statements::Delete d("MyTable");
    ASSERT_EQ(d.string(), "DELETE FROM MyTable;");

    d.where("id = ?");
    ASSERT_EQ(d.string(), "DELETE FROM MyTable WHERE id = ?;");
}