//
// Created by Federico Fuga on 07/04/18.
//

#include <gtest/gtest.h>
#include <clauses.h>
#include <sqlitefieldsop.h>
#include <createstatement.h>
#include <selectstatement.h>
#include "updatestatement.h"

using namespace sqlite;

class UpdateStatements : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    FieldDef<FieldType::Integer> fldId{"id", PrimaryKey};
    FieldDef<FieldType::Integer> fldId2{"id2"};
    FieldDef<FieldType::Text> fldName{"n"};
    FieldDef<FieldType::Integer> fldValue{"v"};

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

        auto create_stmt = makeCreateTableStatement2(db, tablename, fldId, fldId2, fldName, fldValue);
        create_stmt.execute();

        SQLiteStatement stmt(db, statements::Insert(tablename, fldId, fldId2, fldName, fldValue));
        stmt.bind(std::make_tuple(1, 1, "name1", 2));
        stmt.execute();
        stmt.bind(std::make_tuple(2, 2, "name2", 4));
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

    SelectStatement<decltype(fldId)> allZero(fldId);
    allZero.attach(db, tablename);
    Where<decltype(fldValue)> allZeroWhere;
    allZeroWhere.attach(allZero.getStatement(), op::ne(fldValue));
    allZero.where(allZeroWhere);
    allZero.prepare();

    bool f = false;
    allZero.exec([&f](int id) {
        f = true;   // this should not be executed
        return true;
    });
    ASSERT_FALSE(f);

    // update ID=1 value=1
    Where<decltype(fldId)> where;
    where.attach(statement.getStatement(), op::eq(fldId));
    ASSERT_NO_THROW(statement.where(where));
    ASSERT_NO_THROW(statement.prepare());
    ASSERT_NO_THROW(where.bind(1));
    ASSERT_NO_THROW(statement.update(1));


    SelectStatement<decltype(fldValue)> checker(fldValue);
    checker.attach(db, tablename);
    Where<decltype(fldValue)> checkerWhere;
    checkerWhere.attach(checker.getStatement(), op::eq(fldId));
    checker.where(checkerWhere);
    checker.prepare();
    checkerWhere.bind(1);

    f = false;
    checker.exec([&f](int value) {
        f= (value == 1);
        return true;
    });
    ASSERT_TRUE(f);
}

TEST_F(UpdateStatements, updateWithOptional)
{
    UpdateStatement<
            decltype(fldId), decltype(fldId2), decltype(fldName), decltype(fldValue)
    > updateStatement(fldId, fldId2, fldName, fldValue);

    updateStatement.attach(db, tablename);

    Where<decltype(fldId)> where;

    where.attach(updateStatement.getStatement(), op::eq(fldId));
    updateStatement.prepare();

    int id = 1;
    boost::optional<int> id2{20}, value;
    boost::optional<std::string> name;

    ASSERT_NO_THROW(where.bind(id));
    ASSERT_NO_THROW(updateStatement.update(id, id2, name, value));

    SelectStatement<
            decltype(fldId), decltype(fldId2), decltype(fldName), decltype(fldValue)
    > selectStatement(fldId, fldId2, fldName, fldValue);
    selectStatement.attach(db, tablename);
    Where<decltype(fldId)> selectWhere;

    selectWhere.attach(selectStatement.getStatement(), op::eq(fldId));
    selectStatement.where(selectWhere);
    selectStatement.prepare();

    selectWhere.bind(id);
    int rid, rid2, rvalue;
    std::string rname;
    selectStatement.exec([&rid, &rid2, &rname, &rvalue](int id, int id2, std::string name, int value) {
        rid = id;
        rid2 = id2;
        rname = name;
        rvalue = value;
        return true;
    });

    ASSERT_EQ(rid, id);
    ASSERT_EQ(rid2, id2);
    ASSERT_EQ(rname, name);
    ASSERT_EQ(rvalue, value);
}



