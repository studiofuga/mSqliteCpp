//
// Created by Federico Fuga on 19/12/17.
//

#include <sqlitestatementformatters.h>
#include <sqlitefieldsop.h>
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

        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?);");

        ASSERT_NO_THROW(stmt.bind(1, 0));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name"}));
        ASSERT_NO_THROW(stmt.bind(3, 1.5));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?);");

        ASSERT_NO_THROW(stmt.bind(1, 1));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name_1"}));
        ASSERT_NO_THROW(stmt.bind(3, 100.2));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "SELECT id, name, v FROM sample;");

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 2);
    }
}

TEST_F(Statements, createWithStatements)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, value DOUBLE);");
        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, statements::Insert("sample", fldId, fldName, fldValue));
        ASSERT_NO_THROW(stmt.bind(std::make_tuple(0, "first name", -1.1)));
        ASSERT_NO_THROW(stmt.execute());
        ASSERT_NO_THROW(stmt.bind(std::make_tuple(1, "second name", 1.1)));
        ASSERT_NO_THROW(stmt.execute());
        ASSERT_NO_THROW(stmt.bind(std::make_tuple(2, "Third name", 3.1)));
        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, sqlite::statements::Select("sample", fldId, fldName, fldValue));

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count, &stmt]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 3);
    }

    {
        sqlite::statements::Select select("sample", fldId, fldName, fldValue);
        select.where(sqlite::op::eq(fldId));
        SQLiteStatement stmt(db, select);

        stmt.bind(std::make_tuple(1));
        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count, &stmt]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 1);
    }
}


class SelectStatements : public testing::Test
{
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    const FieldDef<FieldType::Integer> fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    const FieldDef<FieldType::Text> fldName = sqlite::makeFieldDef("n", sqlite::FieldType::Text());
    const FieldDef<FieldType::Real> fldValue = sqlite::makeFieldDef("v", sqlite::FieldType::Real());

    SelectStatements() {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();

        SQLiteStatement create_stmt(db, "CREATE TABLE ex (id INTEGER, n TEXT, v DOUBLE);");
        create_stmt.executeStep();

        SQLiteStatement stmt(db, statements::Insert("ex", fldId, fldName, fldValue));
        stmt.bind(std::make_tuple(1, "name1", 2.0));
        stmt.execute();
        stmt.bind(std::make_tuple(2, "name2", 4.0));
        stmt.execute();
        stmt.bind(std::make_tuple(3, "name3", 6.0));
        stmt.execute();
    }
};

TEST_F(SelectStatements, execute)
{
    SQLiteStatement select(db, sqlite::statements::Select("ex", fldId));
    int count = 0;
    select.execute([&count]() {
        ++count;
        return true;
    });

    ASSERT_EQ(count, 3);
}

TEST_F(SelectStatements, join)
{
    FieldDef<FieldType::Integer> jfldId = sqlite::makeFieldDef("jid", sqlite::FieldType::Integer());
    FieldDef<FieldType::Text> jfldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    FieldDef<FieldType::Real> jfldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    SQLiteStatement create_stmt(db, "CREATE TABLE ex2 (jid INTEGER, name TEXT, value DOUBLE);");
    create_stmt.execute();

    SQLiteStatement stmt(db, statements::Insert("ex2", jfldId, jfldName, jfldValue));
    stmt.bind(std::make_tuple(1, "a", -2.0));
    stmt.execute();
    stmt.bind(std::make_tuple(2, "b", -4.0));
    stmt.execute();
    stmt.bind(std::make_tuple(3, "c", -6.0));
    stmt.execute();

    {
        auto s = sqlite::statements::Select("ex", fldId, jfldName, fldValue, jfldValue);
        s.join("ex2", fldId, field("ex2",jfldId));

        std::vector<int> ids;
        std::vector<std::string> names;
        SQLiteStatement stmt(db, s);
        ASSERT_NO_THROW(stmt.execute([&ids, &names, &stmt]() {
            ids.push_back(stmt.getIntValue(0));
            names.push_back(stmt.getStringValue(1));
            return true;
        }));

        ASSERT_EQ(ids.size(), 3);
        ASSERT_EQ(ids[0], 1);
        ASSERT_EQ(names[0], "a");
        ASSERT_EQ(names[1], "b");
        ASSERT_EQ(names[2], "c");
    }
}