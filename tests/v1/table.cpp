//
// Created by Federico Fuga on 11/11/17.
//

#include "msqlitecpp/v1/sqlitestatement.h"
#include "msqlitecpp/v1/sqlitestorage.h"
#include "msqlitecpp/v1/sqlitetable.h"

#include <gtest/gtest.h>

using namespace sqlite;

class V1Table : public ::testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    void SetUp() override {
        db = std::make_shared<SQLiteStorage>(":memory:");
        ASSERT_NO_THROW(db->open());
    }
};

TEST_F(V1Table, creation)
{
    ASSERT_FALSE(db->tableExists("do-not-exists"));

    auto testTable = std::make_tuple(
            makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement(),
            makeFieldDef("name", FieldType::Text())
    );

    ASSERT_THROW(db->dropTable("sample"), SQLiteException);

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "sample", testTable));

    ASSERT_TRUE(db->tableExists("sample"));
}

TEST_F(V1Table, creationWithoutTuple)
{
    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "sample",
                                                makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement(),
                                                makeFieldDef("name", FieldType::Text())
    ));
}

TEST_F(V1Table, query)
{
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
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "sample", testTable));

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
    explicit MyTable(std::shared_ptr<SQLiteStorage> db)
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

TEST_F(V1Table, subclassing)
{
    MyTable myTable(db);

    ASSERT_NO_THROW(myTable.create());
    ASSERT_NO_THROW(myTable.insert(MyTable::Record{"First", 2000}));
    ASSERT_NO_THROW(myTable.insert(MyTable::Record{"Second", 0}));

    std::vector<MyTable::Record> r = myTable.selectAll();
    ASSERT_EQ(r.size(), 2);
}

TEST_F(V1Table, DynamicCreate)
{
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
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "sample", testTable));

    ASSERT_NO_THROW(table.query(std::make_tuple(dynFields[0], dynFields[1]), [](int, int) { }));
}

TEST_F(V1Table, IndexCreate)
{
    auto fldName = makeFieldDef("name", FieldType::Text());
    auto fldX = makeFieldDef("x", FieldType::Real());
    auto fldY = makeFieldDef("y", FieldType::Real());
    auto testTable = std::make_tuple(
            makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement(),
            fldName,
            makeFieldDef("count", FieldType::Integer()),
            fldX,
            fldY
    );

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "sample", testTable));

    ASSERT_NO_THROW(table.createIndex ("name", std::make_tuple(fldName)));
    ASSERT_NO_THROW(table.createIndex ("coords", std::make_tuple(fldX, fldY)));
}

TEST_F(V1Table, InsertWithStatement)
{
    auto fldId = makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement();
    auto fldName = makeFieldDef("name", FieldType::Text());
    auto fldCount = makeFieldDef("count", FieldType::Integer());

    auto testTable = std::make_tuple(
            fldId,
            fldName,
            fldCount
    );

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "sample", testTable));

    auto tb = std::make_tuple(fldName, fldCount);
    SQLiteTable::PreparedInsert<decltype(fldName), decltype(fldCount)> pInsert;
    ASSERT_NO_THROW(pInsert = table.prepareInsert(tb));

    ASSERT_NO_THROW(table.insert(pInsert, std::make_tuple(std::string{"A"},1)));
    ASSERT_NO_THROW(table.insert(pInsert, std::make_tuple(std::string{"B"},2)));
    ASSERT_NO_THROW(table.insert(pInsert, std::make_tuple(std::string{"C"},3)));

    int r = 0;
    ASSERT_NO_THROW(table.query(tb, [&r](std::string name, int value) { ++r; }));
    ASSERT_EQ(r, 3);
}

TEST_F(V1Table, InsertOrUpdate)
{
    auto key = makeFieldDef("Key", FieldType::Text()).primaryKey();
    auto value = makeFieldDef("Value", FieldType::Text());

    auto schema = std::make_tuple(key,value);

    SQLiteTable table;
    ASSERT_NO_THROW(table = SQLiteTable::make(db, "KeyValue", schema));


    SQLiteTable::PreparedInsert<decltype(key), decltype(value)> pInsert;
    ASSERT_NO_THROW(pInsert = table.prepareInsertOrReplace(schema));

    static const std::string vKey {"Key"};
    ASSERT_NO_THROW(table.insert(pInsert, std::make_tuple(vKey,"A")));

    int r = 0;
    std::string v;
    ASSERT_NO_THROW(table.query(schema, [&r,&v](std::string name, std::string value) { ++r; v = value; }));
    ASSERT_EQ(r, 1);
    ASSERT_EQ(v, "A");

    ASSERT_NO_THROW(table.insert(pInsert, std::make_tuple(vKey,"B")));
    r = 0;
    ASSERT_NO_THROW(table.query(schema, [&r,&v](std::string name, std::string value) { ++r; v = value; }));
    ASSERT_EQ(r, 1);
    ASSERT_EQ(v, "B");

}