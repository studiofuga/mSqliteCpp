//
// Created by Federico Fuga on 19/12/17.
//

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
