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
        /*
        auto qfld = std::make_tuple(mName, mValue);
        SQLiteTable::query<decltype(mName),decltype(mValue),std::string, int>
                (qfld, [&r](std::tuple<std::string, int> res) {
            r.push_back(Record{std::get<0>(res), std::get<1>(res)});
        });*/


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
