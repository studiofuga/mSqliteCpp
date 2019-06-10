/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 10/07/18
 */

#include "msqlitecpp/clauses.h"
#include "msqlitecpp/operators.h"

#include <gtest/gtest.h>
#include <msqlitecpp/createstatement.h>
#include <msqlitecpp/insertstatement.h>
#include <msqlitecpp/selectstatement.h>
#include <msqlitecpp/deletestatement.h>
#include <msqlitecpp/updatestatement.h>
#include <msqlitecpp/sqlitefieldsop.h>

using namespace sqlite;

#if defined(WITH_BOOST)

TEST(WhereOptTests, formatOptionalOperators)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    EXPECT_EQ(operators::format(0, operators::eq(fldId),fieldSet), "id = ?1");
    EXPECT_EQ(operators::format(0, operators::eq(fldId),fieldNotSet), "");

    EXPECT_EQ(operators::format(0, operators::ne(fldId),fieldSet), "id <> ?1");
    EXPECT_EQ(operators::format(0, operators::ne(fldId),fieldNotSet), "");

    EXPECT_EQ(operators::format(0, operators::lt(fldId),fieldSet), "id < ?1");
    EXPECT_EQ(operators::format(0, operators::lt(fldId),fieldNotSet), "");

    EXPECT_EQ(operators::format(0, operators::le(fldId),fieldSet), "id <= ?1");
    EXPECT_EQ(operators::format(0, operators::le(fldId),fieldNotSet), "");

    EXPECT_EQ(operators::format(0, operators::gt(fldId),fieldSet), "id > ?1");
    EXPECT_EQ(operators::format(0, operators::gt(fldId),fieldNotSet), "");

    EXPECT_EQ(operators::format(0, operators::ge(fldId),fieldSet), "id >= ?1");
    EXPECT_EQ(operators::format(0, operators::ge(fldId),fieldNotSet), "");
}

TEST(WhereOptTests, formatOptionalUnaryOperator)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto oper = operators::makeOper(fldId, "###");
    EXPECT_EQ(operators::format(0, oper, fieldSet), "id ### ?1");
    EXPECT_EQ(operators::format(0, oper,  fieldNotSet), "");

    // alternate definition
    operators::UnaryOp<decltype(fldId)> oper2(fldId, "!");
    EXPECT_EQ(operators::format(0, oper2, fieldSet), "id ! ?1");
    EXPECT_EQ(operators::format(0, oper2,  fieldNotSet), "");
}

TEST(WhereOptTests, formatOptionalRelationalOperators)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldId2 = sqlite::makeFieldDef("id2", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto opAnd = operators::and_ (operators::eq(fldId), operators::ne(fldId2));
    EXPECT_EQ(opAnd.format(0, fieldSet, fieldSet), "id = ?1 AND id2 <> ?2");
    EXPECT_EQ(opAnd.format(0, fieldSet, fieldNotSet), "id = ?1");
    EXPECT_EQ(opAnd.format(0, fieldNotSet, fieldSet), "id2 <> ?2");
    EXPECT_EQ(opAnd.format(0, fieldNotSet, fieldNotSet), "");
    
    auto opOr = operators::or_ (operators::lt(fldId), operators::ge(fldId2));
    EXPECT_EQ(opOr.format(0, fieldSet, fieldSet), "id < ?1 OR id2 >= ?2");
    EXPECT_EQ(opOr.format(0, fieldSet, fieldNotSet), "id < ?1");
    EXPECT_EQ(opOr.format(0, fieldNotSet, fieldSet), "id2 >= ?2");
    EXPECT_EQ(opOr.format(0, fieldNotSet, fieldNotSet), "");
}

TEST(WhereOptTests, formatWhereCase1)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto w = makeWhereOpt(operators::eq(fldId));

    EXPECT_EQ(w.format(fieldSet), "id = ?1");
    EXPECT_EQ(w.format(fieldNotSet), "1");
}

TEST(WhereOptTests, formatWhereCase2)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldId2 = sqlite::makeFieldDef("id2", sqlite::FieldType::Integer());

    boost::optional<int> fieldSet{1};
    boost::optional<int> fieldNotSet;

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId), operators::eq(fldId2)));

    EXPECT_EQ(w.format(fieldSet, fieldSet), "id = ?1 AND id2 = ?2");
    EXPECT_EQ(w.format(fieldSet, fieldNotSet), "id = ?1");
    EXPECT_EQ(w.format(fieldNotSet, fieldSet), "id2 = ?2");
    EXPECT_EQ(w.format(fieldNotSet, fieldNotSet), "1");
}

class WhereOptTestsWithStatements : public ::testing::Test {
public:
    std::string TableName{"SampleTable"};

    FieldDef<FieldType::Integer> fldId1 {"id1"};
    FieldDef<FieldType::Integer> fldId2 {"id2"};
    FieldDef<FieldType::Integer> fldValue {"value"};

    std::shared_ptr<SQLiteStorage> db = std::make_shared<SQLiteStorage>(":memory:");

protected:
    void SetUp() override
    {
        db->open();

        auto c = makeCreateTableStatement2(db, TableName, fldId1, fldId2, fldValue);
        c.execute();

        auto i = makeInsertStatement(fldId1, fldId2, fldValue);
        i.attach(db, TableName);

        i.insert(1, 1, 1);
        i.insert(1, 2, 2);
        i.insert(2, 2, 3);
        i.insert(2, 3, 4);
    }
};

TEST_F(WhereOptTestsWithStatements, selectWhereCase1)
{
    auto s = makeSelectStatement(fldId1, fldId2, fldValue);
    s.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1;
    boost::optional<int> field2;

    // test 1: select all

    s.where(w.format(field1, field2));
    s.prepare();
    s.bind(field1, field2);

    int count = 0;
    s.exec([&count](int,int,int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 4);
}

TEST_F(WhereOptTestsWithStatements, selectWhereCase2)
{
    auto s = makeSelectStatement(fldValue);
    s.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1{1};
    boost::optional<int> field2;

    // test 2: Select just the first value (id == 1)

    s.where(w.format(field1, field2));
    s.prepare();
    s.bind(field1, field2);

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 2);
}

TEST_F(WhereOptTestsWithStatements, selectWhereCase3)
{
    auto s = makeSelectStatement(fldValue);
    s.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1;
    boost::optional<int> field2{1};

    // test 3: Select just the first value (id2 == 1)

    s.where(w.format(field1, field2));
    s.prepare();
    s.bind(field1, field2);

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 1);
}

TEST_F(WhereOptTestsWithStatements, selectWhereCase4)
{
    auto s = makeSelectStatement(fldValue);
    s.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1{2};
    boost::optional<int> field2{3};

    // test 4: Select a specific combination of 2 fields: id1=2 and id2=3

    s.where(w.format(field1, field2));
    s.prepare();
    s.bind(field1, field2);

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 1);
}

TEST_F(WhereOptTestsWithStatements, updateWhereCase1)
{
    auto u = makeUpdateStatement(fldValue);
    u.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1;
    boost::optional<int> field2;

    u.where(w, field1, field2);
    u.prepare();
    u.bind(field1, field2);

    u.update(1000);

    auto s = makeSelectStatement(fldValue);
    s.attach(db, TableName);
    Where<decltype(fldValue)> sw;
    sw.attach(s.getStatement(), op::eq(fldValue));
    s.where(sw);
    s.prepare();
    sw.bind(1000);

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 4);
}

TEST_F(WhereOptTestsWithStatements, updateWhereCase2)
{
    auto u = makeUpdateStatement(fldValue);
    u.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1 = 1;
    boost::optional<int> field2;

    u.where(w, field1, field2);
    u.prepare();
    u.bind(field1, field2);

    u.update(2000);

    auto s = makeSelectStatement(fldValue);
    s.attach(db, TableName);
    Where<decltype(fldValue)> sw;
    sw.attach(s.getStatement(), op::eq(fldValue));
    s.where(sw);
    s.prepare();
    sw.bind(2000);

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 2);
}

TEST_F(WhereOptTestsWithStatements, updateWhereCase3)
{
    auto u = makeUpdateStatement(fldValue);
    u.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1 = 2;
    boost::optional<int> field2 = 2;

    u.where(w, field1, field2);
    u.prepare();
    u.bind(field1, field2);

    u.update(3000);

    auto s = makeSelectStatement(fldValue);
    s.attach(db, TableName);
    Where<decltype(fldValue)> sw;
    sw.attach(s.getStatement(), op::eq(fldValue));
    s.where(sw);
    s.prepare();
    sw.bind(3000);

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 1);
}

TEST_F(WhereOptTestsWithStatements, DeleteCase1)
{
    auto s = makeSelectStatement(fldId1);
    s.attach(db, TableName);

    DeleteStatement d;
    d.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1;
    boost::optional<int> field2;

    // delete case 1: all => count = 0

    d.where(w.format(field1, field2));
    d.prepare();
    d.bind(field1, field2);
    ASSERT_NO_THROW(d.exec());

    s.prepare();

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 0);
}

TEST_F(WhereOptTestsWithStatements, DeleteCase2)
{
    auto s = makeSelectStatement(fldId1);
    s.attach(db, TableName);

    DeleteStatement d;
    d.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1{1};
    boost::optional<int> field2;

    // delete 2: just id1=1 => results 2

    d.where(w.format(field1, field2));
    d.prepare();
    d.bind(field1, field2);
    ASSERT_NO_THROW(d.exec());

    s.prepare();

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 2);
}

TEST_F(WhereOptTestsWithStatements, DeleteCase3)
{
    auto s = makeSelectStatement(fldId1);
    s.attach(db, TableName);

    DeleteStatement d;
    d.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1;
    boost::optional<int> field2{2};

    // delete case 3: just id2=2 => results 2

    d.where(w.format(field1, field2));
    d.prepare();
    d.bind(field1, field2);
    ASSERT_NO_THROW(d.exec());

    s.prepare();

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 2);
}

TEST_F(WhereOptTestsWithStatements, DeleteCase4)
{
    auto s = makeSelectStatement(fldId1);
    s.attach(db, TableName);

    DeleteStatement d;
    d.attach(db, TableName);

    auto w = makeWhereOpt(operators::and_(operators::eq(fldId1), operators::eq(fldId2)));

    boost::optional<int> field1{2};
    boost::optional<int> field2{3};

    // delete case 3: id1 = 2 AND id2=3 => results 3

    d.where(w.format(field1, field2));
    d.prepare();
    d.bind(field1, field2);
    ASSERT_NO_THROW(d.exec());

    s.prepare();

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 3);
}

TEST_F(WhereOptTestsWithStatements, DeleteCase5)
{
    auto s = makeSelectStatement(fldId1);
    s.attach(db, TableName);

    DeleteStatement d;
    d.attach(db, TableName);

    auto w = makeWhereOpt(operators::or_(operators::eq(fldId1), operators::ne(fldId2)));

    boost::optional<int> field1{1};
    boost::optional<int> field2{3};

    // delete case 5: id1 = 2 AND id2 <> 3 => results 1

    d.where(w.format(field1, field2));
    d.prepare();
    d.bind(field1, field2);
    ASSERT_NO_THROW(d.exec());

    s.prepare();

    int count = 0;
    s.exec([&count](int) {
        ++count;
        return true;
    });

    EXPECT_EQ(count, 1);
}

#endif
