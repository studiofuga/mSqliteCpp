//
// Created by Federico Fuga on 19/12/17.
//

#include "gtest/gtest.h"

#include "sqlitestorage.h"
#include "sqlitestatement.h"

using namespace sqlite;

TEST(statements, create)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");

    ASSERT_NO_THROW(db->open());

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