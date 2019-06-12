//
// Created by Federico Fuga on 07/04/18.
//

#include "msqlitecpp/v1/clauses.h"
#include "msqlitecpp/v1/sqlitefieldsop.h"
#include "msqlitecpp/v1/createstatement.h"
#include "msqlitecpp/v1/selectstatement.h"
#include "msqlitecpp/v1/updatestatement.h"
#include "msqlitecpp/v1/operators.h"

#include <gtest/gtest.h>

using namespace sqlite;

class UpdateStatementsV1 : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    FieldDef<FieldType::Integer> fldId{"id"};
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
        stmt.bind(std::make_tuple(3, 3, "name4", 8));
        stmt.execute();

        Test::SetUp();
    }
};

TEST_F(UpdateStatementsV1, update)
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
        f = (value == 1);
        return true;
    });
    ASSERT_TRUE(f);
}

#if defined(WITH_BOOST)

TEST_F(UpdateStatementsV1, updateWithOptional)
{
    UpdateStatement<
            decltype(fldId2), decltype(fldName), decltype(fldValue)
    > updateStatement(fldId2, fldName, fldValue);

    updateStatement.attach(db, tablename);

    auto where = makeWhere(updateStatement.getStatement(), operators::eq(fldId));

    updateStatement.where(where);

    int id = 1;

    boost::optional<int> id2{20}, value;
    boost::optional<std::string> name;

    ASSERT_NO_THROW(updateStatement.prepare(id2, name, value));
    ASSERT_NO_THROW(where.bind(id));
    ASSERT_NO_THROW(updateStatement.update(id2, name, value));

    SelectStatement<
            decltype(fldId), decltype(fldId2), decltype(fldName), decltype(fldValue)
    > selectStatement(fldId, fldId2, fldName, fldValue);
    selectStatement.attach(db, tablename);
    Where<decltype(fldId)> selectWhere;
    Where<decltype(fldId)> selectWhereOthers;

    selectWhere.attach(selectStatement.getStatement(), operators::eq(fldId)());
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
    ASSERT_EQ(rid2, id2.value());
    ASSERT_EQ(rname, "name1");
    ASSERT_EQ(rvalue, 2);

    // check that the others id2 are NOT 20
    selectWhereOthers.attach(selectStatement.getStatement(), op::ne(fldId));
    selectStatement.where(selectWhereOthers);
    selectStatement.prepare();
    selectWhereOthers.bind(id);

    bool ok = true;
    selectStatement.exec([&ok](int id, int id2, std::string name, int value) {
        if (id != 1 && id2 == 20) {
            ok = false;
        }
        return true;
    });

    ASSERT_TRUE(ok);
}

TEST_F(UpdateStatementsV1, updateWithOptionalMore)
{
    UpdateStatement<
            decltype(fldName), decltype(fldValue)
    > updateStatement(fldName, fldValue);

    updateStatement.attach(db, tablename);

    auto where = makeWhere(updateStatement.getStatement(),
                           operators::and_(operators::eq(fldId), operators::eq(fldId2)));

    updateStatement.where(where);

    int id = 3, id2 = 3;
    boost::optional<int> value = 1000;
    boost::optional<std::string> name{"aaa"};

    ASSERT_NO_THROW(updateStatement.prepare(name, value));
    ASSERT_NO_THROW(where.bind(id, id2));
    ASSERT_NO_THROW(updateStatement.update(name, value));

    SelectStatement<
            decltype(fldId), decltype(fldId2), decltype(fldName), decltype(fldValue)
    > selectStatement(fldId, fldId2, fldName, fldValue);
    selectStatement.attach(db, tablename);
    Where<decltype(fldId)> selectWhere;

    selectWhere.attach(selectStatement.getStatement(), operators::eq(fldId)());
    selectStatement.where(selectWhere);
    selectStatement.prepare();

    selectWhere.bind(id);
    int rid_2, rid2_2, rvalue_2;
    int rid_3, rid2_3, rvalue_3;
    bool ok = true;
    selectStatement.exec(
            [&rid_2, &rid_3, &rid2_2, &rid2_3, &rvalue_2, &rvalue_3, &ok](int id, int id2, std::string name,
                                                                          int value) {
                if (id2 == 2) {
                    rid_2 = id;
                    rid2_2 = id2;
                    rvalue_2 = value;
                } else if (id2 == 3) {
                    rid_3 = id;
                    rid2_3 = id2;
                    rvalue_3 = value;
                } else {
                    ok = false;
                }
                return true;
            });

    ASSERT_TRUE(ok);
    ASSERT_EQ(rid_2, id);
    ASSERT_EQ(rid_3, id);
    ASSERT_EQ(rid2_2, 2);
    ASSERT_EQ(rid2_3, id2);
    ASSERT_EQ(rvalue_2, 6);
    ASSERT_EQ(rvalue_3, 1000);
}
#endif

TEST_F(UpdateStatementsV1, WhereReset)
{
    UpdateStatement<
            decltype(fldValue)
    > updateStatement(fldValue);

    updateStatement.attach(db, tablename);

    auto where = makeWhere(updateStatement.getStatement(), operators::eq(fldId));

    updateStatement.where(where);

    int id = 1;
    int value = 10;

    ASSERT_NO_THROW(updateStatement.prepare());
    ASSERT_NO_THROW(where.bind(id));
    ASSERT_NO_THROW(updateStatement.update(value));

    SelectStatement<
            decltype(fldValue)
    > selectStatement(fldValue);
    selectStatement.attach(db, tablename);
    selectStatement.prepare();

    size_t count = 0;
    selectStatement.exec([&count](int value) {
        if (value == 10) {
            ++count;
        }
        return true;
    });

    ASSERT_EQ(count, 1);

    updateStatement.where();
    ASSERT_NO_THROW(updateStatement.prepare());
    value = 0;
    ASSERT_NO_THROW(updateStatement.update(value));

    count = 0;
    selectStatement.exec([&count](int value) {
        if (value == 0) {
            ++count;
        }
        return true;
    });

    ASSERT_EQ(count, 4);
}


