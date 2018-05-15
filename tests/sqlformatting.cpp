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

    ASSERT_EQ(sqlite::op::and_("1", "2"), "(1 AND 2)");

    ASSERT_EQ(sqlite::op::and_(sqlite::op::lt(fldId),sqlite::op::gt(fldValue)), "(id < ? AND value > ?)");
    ASSERT_EQ(sqlite::op::or_(sqlite::op::lt(fldId),sqlite::op::gt(fldId)), "(id < ? OR id > ?)");

    ASSERT_EQ(sqlite::op::and_(sqlite::op::ne(fldId), sqlite::op::ne(fldId), sqlite::op::ne(fldId)),
                      "(id <> ? AND id <> ? AND id <> ?)");

    ASSERT_EQ(sqlite::op::or_(sqlite::op::eq(fldId), sqlite::op::eq(fldId), sqlite::op::eq(fldId)),
                      "(id = ? OR id = ? OR id = ?)");
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

    auto fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholders(fldId, fldName, fldValue);
    ASSERT_EQ(fieldplaceholders, "id = ?,name = ?,value = ?");

    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholders(fldId);
    ASSERT_EQ(fieldplaceholders, "id = ?");
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

TEST(SqlFormatting, updateStatement)
{
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());
    sqlite::statements::Update u("MyTable",fldName, fldValue);

    ASSERT_EQ(u.string(), "UPDATE MyTable SET name = ?,value = ?;");

    u.where("id = ?");
    ASSERT_EQ(u.string(), "UPDATE MyTable SET name = ?,value = ? WHERE id = ?;");

    u.orAction(sqlite::statements::Update::OrAction::Rollback);
    ASSERT_EQ(u.string(), "UPDATE OR ROLLBACK MyTable SET name = ?,value = ? WHERE id = ?;");
}

TEST(SqlFormatting, ForeignKey)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).primaryKey();
    sqlite::statements::CreateTable::TableConstraint::ForeignKey constraint("u", std::make_tuple(fldId), "x", std::make_tuple("y"));

    ASSERT_EQ(constraint.toString(), "CONSTRAINT u FOREIGN KEY(id) REFERENCES x(y)");

    constraint.onDelete(sqlite::statements::CreateTable::TableConstraint::ForeignKey::Action::Cascade);
    ASSERT_EQ(constraint.toString(), "CONSTRAINT u FOREIGN KEY(id) REFERENCES x(y) ON DELETE CASCADE");

    constraint.onUpdate(sqlite::statements::CreateTable::TableConstraint::ForeignKey::Action::NoAction);
    ASSERT_EQ(constraint.toString(), "CONSTRAINT u FOREIGN KEY(id) REFERENCES x(y) ON DELETE CASCADE ON UPDATE NO ACTION");

    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Integer()).primaryKey();
    sqlite::statements::CreateTable::TableConstraint::ForeignKey c2("w", std::make_tuple(fldId, fldName), "x", std::make_tuple("y", "z"));

    ASSERT_EQ(c2.toString(), "CONSTRAINT w FOREIGN KEY(id,name) REFERENCES x(y,z)");
}

TEST(SqlFormatting, UniqueAndPrimaryKey)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).primaryKey();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());

    sqlite::statements::CreateTable::TableConstraint::Unique c1("u", fldId);
    ASSERT_EQ(c1.toString(), "CONSTRAINT u UNIQUE (id)");

    sqlite::statements::CreateTable::TableConstraint::Unique c2("u", fldId, fldName);
    ASSERT_EQ(c2.toString(), "CONSTRAINT u UNIQUE (id,name)");

    sqlite::statements::CreateTable::TableConstraint::PrimaryKey p1("pk", fldId);
    ASSERT_EQ(p1.toString(), "CONSTRAINT pk PRIMARY KEY (id)");

    sqlite::statements::CreateTable::TableConstraint::PrimaryKey p2("pk", fldId, fldName);
    ASSERT_EQ(p2.toString(), "CONSTRAINT pk PRIMARY KEY (id,name)");
}

TEST(SqlFormatting, createTableStatement)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).primaryKey();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    sqlite::statements::CreateTable c ("MyTable", fldId, fldName, fldValue);
    ASSERT_EQ(c.string(), "CREATE TABLE MyTable (id INTEGER PRIMARY KEY, name TEXT, value REAL);");

    sqlite::statements::CreateTable::TableConstraint::ForeignKey constraint("u", std::make_tuple(fldId), "x", std::make_tuple("y"));
    c.setConstraint(constraint);
    ASSERT_EQ(c.string(), "CREATE TABLE MyTable (id INTEGER PRIMARY KEY, name TEXT, value REAL,"
                          " CONSTRAINT u FOREIGN KEY(id) REFERENCES x(y));");
}

TEST(SqlFormatting, createIndex)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).primaryKey();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());

    sqlite::statements::CreateIndex createIndex1("idx", "mytable", fldId, fldName);
    ASSERT_EQ(createIndex1.string(), "CREATE INDEX idx ON mytable(id,name);");

    sqlite::statements::CreateIndex createIndex2(sqlite::statements::CreateIndex::Unique, "idx", "mytable", fldId, fldName);
    ASSERT_EQ(createIndex2.string(), "CREATE UNIQUE INDEX idx ON mytable(id,name);");
}
