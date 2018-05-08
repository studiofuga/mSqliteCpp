//
// Created by Federico Fuga on 07/04/18.
//

#include <gtest/gtest.h>
#include <clauses.h>
#include <sqlitefieldsop.h>
#include "updatestatement.h"

using namespace sqlite;

class UpdateStatements : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    FieldDef<FieldType::Integer> fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    FieldDef<FieldType::Integer> fldId2 = sqlite::makeFieldDef("id2", sqlite::FieldType::Integer());
    FieldDef<FieldType::Text> fldName = sqlite::makeFieldDef("n", sqlite::FieldType::Text());
    FieldDef<FieldType::Integer> fldValue = sqlite::makeFieldDef("v", sqlite::FieldType::Integer());

    const std::string tablename = "ex";

    int count()
    {
        int c = 0;
        SQLiteStatement select(db, "SELECT id FROM " + tablename + ";");
        select.execute([&c]() {
            ++c;
            return true;
        });

        return c;
    }

protected:
    void SetUp() override
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();

        SQLiteStatement create_stmt(db, "CREATE TABLE " + tablename + " (id INTEGER, id2 INTEGER, n TEXT, v INTEGER);");
        create_stmt.executeStep();

        SQLiteStatement stmt(db, statements::Insert(tablename, fldId, fldId2, fldName, fldValue));
        stmt.bind(std::make_tuple(1, 1, "name1", 2));
        stmt.execute();
        stmt.bind(std::make_tuple(1, 2, "name2", 4));
        stmt.execute();
        stmt.bind(std::make_tuple(3, 2, "name3", 6));
        stmt.execute();

        Test::SetUp();
    }
};

TEST_F(UpdateStatements, update)
{
    UpdateStatement<decltype(fldValue)> statement(fldValue);

    ASSERT_NO_THROW(statement.attach(db, tablename));
    ASSERT_NO_THROW(statement.prepare());
    ASSERT_NO_THROW(statement.update(0));

    SQLiteStatement allzerocheck(db, "SELECT " + fldValue.name() + " FROM " + tablename + ";");

    bool f = false;
    auto allzerochecker = [&allzerocheck, &f]() {
        f = true;
        return allzerocheck.getIntValue(0) == 0;
    };
    ASSERT_TRUE(allzerocheck.execute(allzerochecker));
    ASSERT_TRUE(f);

    Where<decltype(fldId)> where(statement.getStatement(), op::eq(fldId));

    ASSERT_NO_THROW(statement.where(where));
    ASSERT_NO_THROW(statement.prepare());
    ASSERT_NO_THROW(where.bind(1));
    ASSERT_NO_THROW(statement.update(1));

    SQLiteStatement checker(db, "SELECT " + fldId.name() + "," + fldValue.name() + " FROM " + tablename + ";");
    auto onechecker = [&checker, &f]() {
        std::cout << "Id: " << checker.getIntValue(0) << " value " << checker.getIntValue(1) << "\n";
        f = true;
        auto v = checker.getIntValue(1);
        if (checker.getIntValue(0) == 1) {
            if (v != 1) {
                std::cout << "Id 1 should have value 1, has " << v << " instead\n";
            }
            return v == 1;
        }
        if (v != 0) {
            std::cout << "Expected value 0, " << v << " instead\n";
        }
        return v == 0;
    };

    f = false;
    ASSERT_TRUE(checker.execute(onechecker));
    ASSERT_TRUE(f);
}