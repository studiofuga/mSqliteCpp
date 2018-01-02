//
// Created by Federico Fuga on 19/12/17.
//

#include <sqlitestatementformatters.h>
#include "gtest/gtest.h"

#include "sqlitestorage.h"
#include "sqlitestatement.h"

using namespace sqlite;

class Statements : public testing::Test
{
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    Statements() {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();
    }
};

TEST_F(Statements, create)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, v DOUBLE);");

        ASSERT_NO_THROW(stmt.execute([](){ return true; }));
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?);");

        ASSERT_NO_THROW(stmt.bind(1, 0));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name"}));
        ASSERT_NO_THROW(stmt.bind(3, 1.5));

        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, "SELECT id, name, v FROM sample;");

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count]() { ++count; return true;}));

        ASSERT_EQ(count, 1);

    }
}

TEST_F(Statements, createWithStatements)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, value DOUBLE);");

        ASSERT_NO_THROW(stmt.execute([](){ return true; }));
    }

    {
        SQLiteStatement stmt(db, statements::Insert("sample", fldId, fldName, fldValue));

        ASSERT_NO_THROW(stmt.bind(1, 0));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name"}));
        ASSERT_NO_THROW(stmt.bind(3, 1.5));

        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, statements::Insert("sample", fldId, fldName, fldValue));
        stmt.bind(std::make_tuple(0, "name", 1.0));
        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, sqlite::statements::Select("sample", fldId, fldName, fldValue));

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count]() { ++count; return true;}));

        ASSERT_EQ(count, 1);
    }
}

TEST_F(Statements, execute)
{
    {
        SQLiteStatement create_stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, v DOUBLE);");
        ASSERT_NO_THROW(create_stmt.execute());
    }

    {
        SQLiteStatement insert_stmt(db, "INSERT INTO sample VALUES (1,\"name\",1.5);");
        ASSERT_NO_THROW(insert_stmt.execute());
    }

    {
        SQLiteStatement insert_stmt(db, "INSERT INTO sample VALUES (2,\"name2\",1.5);");
        ASSERT_NO_THROW(insert_stmt.execute());
    }

    SQLiteStatement select(db, "SELECT id,v FROM sample;");
    int count = 0;
    while (select.execute([&count]() { ++count; return true; }));

    ASSERT_EQ(count, 2);
}
