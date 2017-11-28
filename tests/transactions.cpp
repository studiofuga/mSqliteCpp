//
// Created by Federico Fuga on 28/11/17.
//

#include <gtest/gtest.h>

#include "sqlitetransaction.h"
#include "sqlitetable.h"
#include "sqlitestorage.h"

using namespace sqlite;

class TransactionTest : public ::testing::Test {
    std::shared_ptr<SQLiteStorage> mDb;
public:
    TransactionTest() {
        mDb = std::make_shared<SQLiteStorage>(":memory:");

        mDb->open();
    }

    FieldDef<FieldType::Integer> fldId = makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement();
    FieldDef<FieldType::Text> fldName = makeFieldDef("name", FieldType::Text());

    auto db() const { return mDb; }
};

TEST_F (TransactionTest, commit)
{
    SQLiteTable table(db(), "test");

    ASSERT_NO_THROW(table.create(std::make_tuple(fldId, fldName)));

    SQLiteTransaction transaction(db());
    ASSERT_NO_THROW(table.insert(fldName.assign("aaaa")));
    transaction.commit();

    int r = 0;
    ASSERT_NO_THROW(table.query(std::make_tuple(fldName), [&r](std::string name) { ++r; }));
    ASSERT_EQ(r, 1);

    {
        SQLiteTransaction t2(db(), SQLiteTransaction::DestructorAction::Commit);
        ASSERT_NO_THROW(table.insert(fldName.assign("bbbb")));
    }

    r = 0;
    ASSERT_NO_THROW(table.query(std::make_tuple(fldName), [&r](std::string name) { ++r; }));
    ASSERT_EQ(r, 2);
}

TEST_F (TransactionTest, abort)
{
    SQLiteTable table(db(), "test2");

    ASSERT_NO_THROW(table.create(std::make_tuple(fldId, fldName)));

    SQLiteTransaction transaction(db());
    ASSERT_NO_THROW(table.insert(fldName.assign("aaaa")));
    transaction.abort();

    int r = 0;
    ASSERT_NO_THROW(table.query(std::make_tuple(fldName), [&r](std::string name) { ++r; }));
    ASSERT_EQ(r, 0);

    // Destructor action
    {
        SQLiteTransaction t2(db(), SQLiteTransaction::DestructorAction::Abort);
        ASSERT_NO_THROW(table.insert(fldName.assign("bbbb")));
    }

    r = 0;
    ASSERT_NO_THROW(table.query(std::make_tuple(fldName), [&r](std::string name) { ++r; }));
    ASSERT_EQ(r, 0);

    // Default destructor action is abort
    {
        SQLiteTransaction t2(db());
        ASSERT_NO_THROW(table.insert(fldName.assign("bbbb")));
    }

    r = 0;
    ASSERT_NO_THROW(table.query(std::make_tuple(fldName), [&r](std::string name) { ++r; }));
    ASSERT_EQ(r, 0);
}
