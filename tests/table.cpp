//
// Created by Federico Fuga on 11/11/17.
//

#include <gtest/gtest.h>

#include "sqlitestorage.h"
#include "sqlitetable.h"

using namespace sqlite;

TEST(table, creation)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");

    ASSERT_NO_THROW(db->open());
    ASSERT_FALSE(db->tableExists("do-not-exists"));

    auto testTable = std::make_tuple(
            makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement(),
            makeFieldDef("name", FieldType::Text())
    );

    ASSERT_THROW(db->dropTable("sample"), SQLiteException);

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::create(db, "sample", testTable));

    ASSERT_TRUE(db->tableExists("sample"));
}

TEST(table, query)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");

    ASSERT_NO_THROW(db->open());
    ASSERT_FALSE(db->tableExists("do-not-exists"));

    auto fldId = makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement();
    auto fldName = makeFieldDef("name", FieldType::Text());
    auto fldCount = makeFieldDef("count", FieldType::Integer());
    auto testTable = std::make_tuple(
            fldId,
            fldName,
            fldCount
    );

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::create(db, "sample", testTable));

    auto tb = std::make_tuple(fldName, fldCount);
    ASSERT_NO_THROW(table.insert(tb, std::make_tuple(std::string{"first"}, 0)));
    ASSERT_NO_THROW(table.insert(tb, std::make_tuple(std::string{"second"}, 100)));

    // Alternative signature for insert
    ASSERT_NO_THROW(table.insert(fldName.assign("third"), fldCount.assign(250)));

    int r = 0;
    ASSERT_NO_THROW(table.query(tb, [&r](std::string name, int value) { ++r; }));
    ASSERT_EQ(r, 3);

    r = 0;
    auto where = std::make_tuple(fldCount.assign(100));
    ASSERT_NO_THROW(table.query(tb, where, [&r](std::string name, int value) { ++r; }));
    ASSERT_EQ(r, 1);

    // update
    ASSERT_NO_THROW(table.update(std::make_tuple(fldCount.assign(100)), std::make_tuple(fldId.assign(1))));

    r = 0;
    ASSERT_NO_THROW(table.query(tb, where, [&r](std::string name, int value) { ++r; }));
    ASSERT_EQ(r, 2);
}

class MyTable : public SQLiteTable {
private:
    FieldDef<FieldType::Integer> mId = makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement();
    FieldDef<FieldType::Text> mName = makeFieldDef("name", FieldType::Text());
    FieldDef<FieldType::Integer> mValue = makeFieldDef("value", FieldType::Integer());
public:
    MyTable(std::shared_ptr<SQLiteStorage> db)
            : SQLiteTable(db, "records")
    {
    }

    void create() {
        SQLiteTable::create(std::make_tuple(mId, mName, mValue));
    }

    struct Record {
        std::string name;
        int value;
    };

    void insert(const Record &record) {
        SQLiteTable::insert(std::make_tuple(mName, mValue), std::make_tuple(record.name, record.value));
    }

    std::vector<Record> selectAll() {
        std::vector<Record> r;
        SQLiteTable::query(std::make_tuple(mName, mValue), [&r](std::string name, int value) {
            r.push_back(Record{name, value});
        });

        return r;
    }
};

TEST(table, subclassing)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");

    ASSERT_NO_THROW(db->open());

    MyTable myTable(db);

    ASSERT_NO_THROW(myTable.create());
    ASSERT_NO_THROW(myTable.insert(MyTable::Record{"First", 2000}));
    ASSERT_NO_THROW(myTable.insert(MyTable::Record{"Second", 0}));

    std::vector<MyTable::Record> r = myTable.selectAll();
    ASSERT_EQ(r.size(), 2);
}

TEST(table, DynamicCreate)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");

    ASSERT_NO_THROW(db->open());

    std::vector<FieldDef<FieldType::Integer>> dynFields {
            makeFieldDef("c1", FieldType::Integer()),
            makeFieldDef("c2", FieldType::Integer()),
            makeFieldDef("c3", FieldType::Integer()),
            makeFieldDef("total", FieldType::Integer())
    };

    auto testTable = std::make_tuple(
            makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement(),
            makeFieldDef("name", FieldType::Text()),
            dynFields
    );

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::create(db, "sample", testTable));

    ASSERT_NO_THROW(table.query(std::make_tuple(dynFields[0], dynFields[1]), [](int, int) { }));
}