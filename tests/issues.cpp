//
// Created by Federico Fuga on 16/12/17.
//

#include <gtest/gtest.h>

#include <sqlitestorage.h>
#include <sqlitetable.h>

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

        auto tbl = SQLiteTable::create(db1, "mytable", t);
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