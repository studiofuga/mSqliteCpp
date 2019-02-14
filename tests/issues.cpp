//
// Created by Federico Fuga on 16/12/17.
//

#include <gtest/gtest.h>

#include <sqlitestorage.h>
#include <sqlitetable.h>
#include <deletestatement.h>
#include <insertstatement.h>
#include <selectstatement.h>
#include <createstatement.h>

using namespace sqlite;

TEST(issues, lockAfterClose)
{
    auto dbname = "lockissue.db";
    {
        auto db1 = std::make_shared<SQLiteStorage>(dbname);
        ASSERT_NO_THROW(db1->open());

        if (db1->tableExists("mytable"))
            ASSERT_NO_THROW(db1->dropTable("mytable"));

        auto fldId = makeFieldDef("id", FieldType::Integer());
        auto fldName = makeFieldDef("name", FieldType::Text());
        auto t = std::make_tuple(
                fldId,
                fldName
        );

        auto tbl = SQLiteTable::make(db1, "mytable", t);
        ASSERT_NO_THROW(tbl.insert(t, std::make_tuple(1, std::string{"aaa"})));

        tbl.query(std::make_tuple(fldId), [](int x) {});
        ASSERT_NO_THROW(db1->close());

        // Issue: Now reopen the db and it shouldn't be locked
    }

    {
        auto db1 = std::make_shared<SQLiteStorage>(dbname);
        ASSERT_NO_THROW(db1->open());
        if (db1->tableExists("mytable"))
            ASSERT_NO_THROW(db1->dropTable("mytable"));

        ASSERT_NO_THROW(db1->close());
    }
}

TEST(issues, insertOptionalUnboundValues)
{
    //auto db1 = std::make_shared<SQLiteStorage>("issuestest.db");
    auto db1 = std::make_shared<SQLiteStorage>(":memory:");
    ASSERT_NO_THROW(db1->open());

    auto fldId = makeFieldDef("id", FieldType::Integer());
    auto fldName = makeFieldDef("name", FieldType::Text());
    auto fldValue1 = makeFieldDef("value1", FieldType::Integer());
    auto fldValue2 = makeFieldDef("value2", FieldType::Integer());
    auto t = std::make_tuple(fldId, fldName, fldValue1, fldValue2);

    auto tbl = SQLiteTable::make(db1, "mytable", t);
    makeCreateUniqueIndexStatement(db1, "myindex", "mytable", fldId).execute();

    ASSERT_NO_THROW(tbl.insert(t, std::make_tuple(1, std::string{"aaa"}, 1, 1)));
    ASSERT_NO_THROW(tbl.insert(t, std::make_tuple(2, std::string{"xxx"}, 2, 2)));

    sqlite::DeleteStatement s;

    s.attach(db1, "mytable");
    s.prepare();
    ASSERT_NO_THROW(s.exec());

    auto insertStatement = makeInsertStatement(fldId, fldName, fldValue1, fldValue2);
    insertStatement.attach(db1, "mytable");

    ASSERT_NO_THROW(insertStatement.insert(1, boost::optional<std::string>(), boost::optional<int>(), boost::optional<int>(5)));

    auto selectStatement = makeSelectStatement(fldId, fldName, fldValue1, fldValue2);

    selectStatement.attach(db1, "mytable");
    selectStatement.prepare();

    int id, value1, value2;
    std::string name;
    int count = 0;

    auto queryFunction = [&id, &value1, &value2, &name, &count](int f1, std::string s, int f2, int f3) {
        ++count;
        id = f1;
        name = s;
        value1 = f2;
        value2 = f3;
        return true;
    };

    ASSERT_NO_THROW(selectStatement.exec(queryFunction));

    EXPECT_EQ(count, 1);
    EXPECT_EQ(id, 1);
    EXPECT_EQ(name, "");
    EXPECT_EQ(value1, 0);
    EXPECT_EQ(value2, 5);
}
