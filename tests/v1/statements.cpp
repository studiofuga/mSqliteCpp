//
// Created by Federico Fuga on 19/12/17.
//

#include "msqlitecpp/v1/insertstatement.h"
#include "msqlitecpp/v1/deletestatement.h"
#include "msqlitecpp/v1/sqlitestorage.h"
#include "msqlitecpp/v1/sqlitestatement.h"
#include "msqlitecpp/v1/selectstatement.h"
#include "msqlitecpp/v1/createstatement.h"

#include "msqlitecpp/v1/sqlitestatementformatters.h"
#include "msqlitecpp/v1/sqlitefieldsop.h"
#include "msqlitecpp/v1/clauses.h"

#include "gtest/gtest.h"

using namespace sqlite;

class StatementsV1 : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    StatementsV1()
            : fldId("id", sqlite::NotNull),
              fldName("name", sqlite::NotNull),
              fldValue("v", sqlite::NotNull)
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();
    }

    sqlite::FieldDef<sqlite::FieldType::Integer> fldId;
    sqlite::FieldDef<sqlite::FieldType::Text> fldName;
    sqlite::FieldDef<sqlite::FieldType::Real> fldValue;
};

TEST_F(StatementsV1, CreateTable)
{
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create(db, "sample", fldId, fldName,
                                                                                        fldValue);
    ASSERT_NO_THROW(create.execute());

    sqlite::FieldDef<sqlite::FieldType::Integer> fid2("id2");
    sqlite::FieldDef<sqlite::FieldType::Text> ftext("text");
    CreateTableStatement<decltype(fid2), decltype(ftext)> cdep(db, "other", fid2, ftext);
    statements::CreateTable::TableConstraint::ForeignKey fkey("u", std::make_tuple(fid2), "other",
                                                              std::make_tuple(fldId));
    cdep.setTableConstraint(fkey);
    ASSERT_NO_THROW(cdep.execute());

    ASSERT_EQ(cdep.statementString(),
              "CREATE TABLE other (id2 INTEGER, text TEXT, CONSTRAINT u FOREIGN KEY(id2) REFERENCES other(id))"
    );

}

TEST_F(StatementsV1, CreateTableWithConstraint)
{
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> c1(db, "withunique", fldId, fldName,
                                                                                    fldValue);

    statements::CreateTable::TableConstraint::Unique unique("uniqueKey", fldId, fldName);
    c1.setTableConstraint(unique);
    ASSERT_NO_THROW(c1.execute());

    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> c2(db, "withprimarykey", fldId, fldName,
                                                                                    fldValue);

    statements::CreateTable::TableConstraint::PrimaryKey primaryKey("pkey", fldId, fldName);
    c2.setTableConstraint(primaryKey);
    ASSERT_NO_THROW(c2.execute());

}

TEST_F(StatementsV1, MakeCreateTableStatementOpt1)
{
    auto createTable = makeCreateTableStatement(fldId, fldName, fldValue);
    createTable.attach(db, "make1");

    ASSERT_NO_THROW(createTable.execute());
}

TEST_F(StatementsV1, MakeCreateTableStatementOpt2)
{
    auto createTable = makeCreateTableStatement2(db, "make2", fldId, fldName, fldValue);
    ASSERT_NO_THROW(createTable.execute());
}

TEST_F(StatementsV1, FixCreateTableInvertedAttachConstraint)
{
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create(fldId, fldName,
                                                                                        fldValue);

    statements::CreateTable::TableConstraint::Unique unique("u", fldId, fldName);

    create.attach(db, "sample");
    create.setTableConstraint(unique);

    ASSERT_EQ(create.statementString(),
              "CREATE TABLE sample (id INTEGER NOT NULL, name TEXT NOT NULL, v REAL NOT NULL, CONSTRAINT u UNIQUE (id,name))"
    );

    // here we invert the two functions

    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create2(fldId, fldName,
                                                                                         fldValue);

    statements::CreateTable::TableConstraint::Unique unique2("u2", fldId, fldName);

    create2.setTableConstraint(unique2);
    create2.attach(db, "sample");

    ASSERT_EQ(create2.statementString(),
              "CREATE TABLE sample (id INTEGER NOT NULL, name TEXT NOT NULL, v REAL NOT NULL, CONSTRAINT u2 UNIQUE (id,name))"
    );
}


TEST_F(StatementsV1, FixCreateTableMultipleConstraint)
{
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create(fldId, fldName,
                                                                                        fldValue);

    statements::CreateTable::TableConstraint::Unique unique("u", fldId, fldName);
    statements::CreateTable::TableConstraint::Unique unique2("u2", fldValue);

    create.attach(db, "sample");
    create.setTableConstraint(unique);
    create.setTableConstraint(unique2);

    ASSERT_EQ(create.statementString(),
              "CREATE TABLE sample (id INTEGER NOT NULL, name TEXT NOT NULL, v REAL NOT NULL, CONSTRAINT u UNIQUE (id,name),"
              " CONSTRAINT u2 UNIQUE (v))"
    );
}

TEST_F(StatementsV1, createIndex)
{
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create(fldId, fldName,
                                                                                        fldValue);
    create.attach(db, "mytable");
    ASSERT_NO_THROW(create.execute());

    CreateIndexStatement<decltype(fldId), decltype(fldName)> createIndex1("idx", fldId, fldName);

    createIndex1.attach(db, "mytable");
    ASSERT_NO_THROW(createIndex1.execute());

    CreateIndexStatement<decltype(fldId), decltype(fldName)> createIndex2("unqIdx", fldId, fldName);

    createIndex2.attach(db, "mytable").unique();
    ASSERT_EQ(createIndex2.statementString(), "CREATE UNIQUE INDEX unqIdx ON mytable(id,name)");
    ASSERT_NO_THROW(createIndex2.execute());

    auto ind3 = makeCreateIndexStatement(db, "idx3", "mytable", fldId, fldName);
    ASSERT_EQ(ind3.statementString(), "CREATE INDEX idx3 ON mytable(id,name)");
    ASSERT_NO_THROW(ind3.execute());

    auto uq = makeCreateUniqueIndexStatement(db, "unqIdx4", "mytable", fldId, fldName);
    ASSERT_EQ(uq.statementString(), "CREATE UNIQUE INDEX unqIdx4 ON mytable(id,name)");
    ASSERT_NO_THROW(uq.execute());
}

TEST_F(StatementsV1, create)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, v DOUBLE)");

        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?)");

        ASSERT_NO_THROW(stmt.bind(1, 0));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name"}));
        ASSERT_NO_THROW(stmt.bind(3, 1.5));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?)");

        ASSERT_NO_THROW(stmt.bind(1, 1));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name_1"}));
        ASSERT_NO_THROW(stmt.bind(3, 100.2));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "SELECT id, name, v FROM sample");

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 2);
    }
}

TEST_F(StatementsV1, typedCreate)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text()).notNull();
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real()).notNull();

    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT NOT NULL, value DOUBLE NOT NULL)");
        ASSERT_NO_THROW(stmt.execute());
    }

    auto insertStatement = sqlite::makeInsertStatement(fldId, fldName, fldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, "sample"));
    ASSERT_NO_THROW(insertStatement.insert(1, std::string{"first"}, 10.0));
    ASSERT_NO_THROW(insertStatement.insert(2, std::string{"second"}, 20.0));
    ASSERT_THROW(insertStatement.insert(1, std::string{"First Again"}, 1.0), sqlite::SQLiteException);

    auto insertOrReplaceStatement = sqlite::makeInsertStatement(fldId, fldName, fldValue);
    ASSERT_NO_THROW(insertOrReplaceStatement.replaceOnConflict());
    ASSERT_NO_THROW(insertOrReplaceStatement.attach(db, "sample"));
    ASSERT_NO_THROW(insertOrReplaceStatement.insert(1, std::string{"First Again"}, 1.0));
}

TEST_F(StatementsV1, statementExecuteFail)
{
    SQLiteStatement stmt(db,
                         "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT)");
    ASSERT_NO_THROW(stmt.execute());

    stmt.attach(db,
                "INSERT INTO sample VALUES (0, 'AAA'),(1,'BBB'),(2,'CCC')");
    ASSERT_NO_THROW(stmt.execute());

    // fails at mid
    stmt.attach(db, "SELECT * from SAMPLE");
    ASSERT_TRUE(stmt.execute());
    ASSERT_FALSE(stmt.execute([]() {
        return false;
    }));
    int c = 0;
    ASSERT_FALSE(stmt.execute([&c]() {
        ++c;
        if (c == 2) {
            return false;
        }
        return true;
    }));

}

TEST_F(StatementsV1, insertFromQuery)
{
    SQLiteStatement stmt(db,
                         "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT)");
    ASSERT_NO_THROW(stmt.execute());

    stmt.attach(db,
                "INSERT INTO sample VALUES (0, 'AAA'),(1,'BBB'),(2,'CCC')");
    ASSERT_NO_THROW(stmt.execute());

    stmt.attach(db,
                "CREATE TABLE newsample (id INTEGER PRIMARY KEY, relid INTEGER, name TEXT)");
    ASSERT_NO_THROW(stmt.execute());


    stmt.attach(db,
                "INSERT INTO newsample (relid, name) SELECT  id,? FROM sample WHERE id=?");

    ASSERT_NO_THROW(stmt.bind(1, "aiuto"));
    ASSERT_NO_THROW(stmt.bind(2, 0));

    ASSERT_NO_THROW(stmt.execute());
}

TEST_F(StatementsV1, selectStatementsV11)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text()).notNull();
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real()).notNull();

    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT NOT NULL, value DOUBLE NOT NULL)");
        ASSERT_NO_THROW(stmt.execute());
    }

    auto insertStatement = sqlite::makeInsertStatement(fldId, fldName, fldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, "sample"));
    ASSERT_NO_THROW(insertStatement.insert(1, std::string{"first"}, 10.0));

    SelectStatement<
            decltype(fldId), decltype(fldValue)>
            selectStatement(fldId, fldValue);

    ASSERT_NO_THROW(selectStatement.attach(db, "sample"));
    ASSERT_NO_THROW(selectStatement.prepare());

    int count = 0;
    int n = 0;
    double v = 0;
    ASSERT_NO_THROW(selectStatement.exec([&n, &v, &count](int rn, double rv) {
        n = rn;
        v = rv;
        count++;
        return true;
    }));

    ASSERT_EQ(count, 1);
    ASSERT_EQ(n, 1);
    ASSERT_EQ(v, 10.0);

    // Insert another row
    ASSERT_NO_THROW(insertStatement.insert(2, std::string{"second"}, 20.0));

    // Count again
    count = 0;
    ASSERT_NO_THROW(selectStatement.exec([&count](int rn, double rv) {
        count++;
        return true;
    }));
    ASSERT_EQ(count, 2);

    // Now we add a where clause

    Where<decltype(fldId)> where(selectStatement.getStatement(), op::eq(fldId));
    ASSERT_NO_THROW(selectStatement.where(where));
    ASSERT_NO_THROW(selectStatement.prepare());

    ASSERT_NO_THROW(where.bind(int(1)));

    count = 0;
    n = 0;
    v = 0;
    ASSERT_NO_THROW(selectStatement.exec([&n, &v, &count](int rn, double rv) {
        n = rn;
        v = rv;
        count++;
        return true;
    }));

    ASSERT_EQ(count, 1);
    ASSERT_EQ(n, 1);
    ASSERT_EQ(v, 10.0);

    // Insert another row
    ASSERT_NO_THROW(insertStatement.insert(3, std::string{"second"}, 20.0));

    SelectStatement<
            decltype(fldName), decltype(fldValue)>
            selectStatement2(fldName, fldValue);

    ASSERT_NO_THROW(selectStatement2.attach(db, "sample"));
    selectStatement2.groupBy(fldName);
    ASSERT_NO_THROW(selectStatement2.prepare());

    count = 0;
    v = 0;
    ASSERT_NO_THROW(selectStatement2.exec([&v, &count](std::string nm, double rv) {
        if (nm == "second") {
            v = rv;
        }
        count++;
        return true;
    }));

    ASSERT_EQ(count, 2);
    ASSERT_EQ(v, 20.0);

    // now remove the where clause
    ASSERT_NO_THROW(selectStatement.where());
    ASSERT_NO_THROW(selectStatement.prepare());
    count = 0;
    ASSERT_NO_THROW(selectStatement.exec([&count](int rn, double rv) {
        count++;
        return true;
    }));

    ASSERT_EQ(count, 3);
}

TEST_F(StatementsV1, selectStatementsV1Sorting)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldNum = sqlite::makeFieldDef("num", sqlite::FieldType::Integer()).notNull();
    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER, num INTEGER)");
        ASSERT_NO_THROW(stmt.execute());
    }

    auto insertStatement = sqlite::makeInsertStatement(fldId, fldNum);
    ASSERT_NO_THROW(insertStatement.attach(db, "sample"));
    ASSERT_NO_THROW(insertStatement.insert(3, 4));
    ASSERT_NO_THROW(insertStatement.insert(1, 1));
    ASSERT_NO_THROW(insertStatement.insert(2, 2));

    SelectStatement<decltype(fldId)> selectStatement(fldId);

    ASSERT_NO_THROW(selectStatement.attach(db, "sample"));
    ASSERT_NO_THROW(selectStatement.orderBy(fldId).orderBy(Ordering::ASC));
    ASSERT_NO_THROW(selectStatement.prepare());

    std::vector<int> in;
    ASSERT_NO_THROW(selectStatement.exec([&in](int rn) {
        in.push_back(rn);
        return true;
    }));

    ASSERT_EQ(in.size(), 3);
    EXPECT_EQ(in.at(0), 1);
    EXPECT_EQ(in.at(1), 2);
    EXPECT_EQ(in.at(2), 3);

    ASSERT_NO_THROW(selectStatement.orderBy(fldId).orderBy(Ordering::DESC));
    ASSERT_NO_THROW(selectStatement.prepare());

    in.clear();
    ASSERT_NO_THROW(selectStatement.exec([&in](int rn) {
        in.push_back(rn);
        return true;
    }));

    ASSERT_EQ(in.size(), 3);
    EXPECT_EQ(in.at(0), 3);
    EXPECT_EQ(in.at(1), 2);
    EXPECT_EQ(in.at(2), 1);

    ASSERT_NO_THROW(selectStatement.orderBy(fldId));
    ASSERT_NO_THROW(selectStatement.prepare());

    in.clear();
    ASSERT_NO_THROW(selectStatement.exec([&in](int rn) {
        in.push_back(rn);
        return true;
    }));

    ASSERT_EQ(in.size(), 3);
    EXPECT_EQ(in.at(0), 1);
    EXPECT_EQ(in.at(1), 2);
    EXPECT_EQ(in.at(2), 3);

    ASSERT_NO_THROW(selectStatement.orderBy());
    ASSERT_NO_THROW(selectStatement.prepare());

    in.clear();
    ASSERT_NO_THROW(selectStatement.exec([&in](int rn) {
        in.push_back(rn);
        return true;
    }));

    ASSERT_EQ(in.size(), 3);
    EXPECT_EQ(in.at(0), 3);
    EXPECT_EQ(in.at(1), 1);
    EXPECT_EQ(in.at(2), 2);

    // Multiple sorting
    ASSERT_NO_THROW(insertStatement.insert(2, 0));

    SelectStatement<decltype(fldId), decltype(fldNum)> selectStatement2(fldId, fldNum);

    ASSERT_NO_THROW(selectStatement2.attach(db, "sample"));
    ASSERT_NO_THROW(selectStatement2.orderBy(fldId, fldNum).orderBy(Ordering::ASC));
    ASSERT_NO_THROW(selectStatement2.prepare());

    in.clear();
    ASSERT_NO_THROW(selectStatement2.exec([&in](int rn, int num) {
        in.push_back(num);
        return true;
    }));

    ASSERT_EQ(in.size(), 4);
    EXPECT_EQ(in.at(0), 1);
    EXPECT_EQ(in.at(1), 0);
    EXPECT_EQ(in.at(2), 2);
    EXPECT_EQ(in.at(3), 4);
}

TEST_F(StatementsV1, selectStatementsV1MultipleBind)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text()).notNull();
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real()).notNull();

    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT NOT NULL, value DOUBLE NOT NULL)");
        ASSERT_NO_THROW(stmt.execute());
    }

    auto insertStatement = sqlite::makeInsertStatement(fldId, fldName, fldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, "sample"));
    ASSERT_NO_THROW(insertStatement.insert(1, std::string{"first"}, 10.0));
    ASSERT_NO_THROW(insertStatement.insert(2, std::string{"sec"}, 20.0));
    ASSERT_NO_THROW(insertStatement.insert(3, std::string{"third"}, 30.0));

    SelectStatement<
            decltype(fldId), decltype(fldValue)>
            selectStatement(fldId, fldValue);

    ASSERT_NO_THROW(selectStatement.attach(db, "sample"));

    Where<decltype(fldId)> where(selectStatement.getStatement(), "id IN (?,?)");
    ASSERT_NO_THROW(selectStatement.where(where));
    ASSERT_NO_THROW(selectStatement.prepare());

    ASSERT_NO_THROW(where.bind(1,2));

    std::vector<std::tuple<int,double>> r;
    EXPECT_NO_THROW(selectStatement.exec([&r](int rn, double rv) {
        r.push_back(std::make_tuple(rn,rv));
        return true;
    }));

    EXPECT_EQ(r.size(), 2);
    EXPECT_EQ(std::get<0>(r[0]), 1);
    EXPECT_EQ(std::get<0>(r[1]), 2);
}

TEST_F(StatementsV1, selectStatementsV1MultipleBindDynamic)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text()).notNull();
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real()).notNull();

    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT NOT NULL, value DOUBLE NOT NULL)");
        ASSERT_NO_THROW(stmt.execute());
    }

    auto insertStatement = sqlite::makeInsertStatement(fldId, fldName, fldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, "sample"));
    ASSERT_NO_THROW(insertStatement.insert(1, std::string{"first"}, 10.0));
    ASSERT_NO_THROW(insertStatement.insert(2, std::string{"sec"}, 20.0));
    ASSERT_NO_THROW(insertStatement.insert(3, std::string{"third"}, 30.0));

    SelectStatement<
            decltype(fldId), decltype(fldValue)>
            selectStatement(fldId, fldValue);

    ASSERT_NO_THROW(selectStatement.attach(db, "sample"));

    std::vector<int> whereValues {1,2,3};

    std::ostringstream ss;
    ss << "id IN (?";
    for (int i = 0; i < whereValues.size()-1; ++i) {
        ss << ",?";
    }
    ss << ")";

    Where<decltype(fldId)> where(selectStatement.getStatement(), ss.str());
    ASSERT_NO_THROW(selectStatement.where(where));
    ASSERT_NO_THROW(selectStatement.prepare());

    for (size_t i = 0; i < whereValues.size(); ++i) {
        where.bindN(i+1, whereValues[i]);
    }

    std::vector<std::tuple<int,double>> r;
    EXPECT_NO_THROW(selectStatement.exec([&r](int rn, double rv) {
        r.push_back(std::make_tuple(rn,rv));
        return true;
    }));

    EXPECT_EQ(r.size(), 3);
    EXPECT_EQ(std::get<0>(r[0]), 1);
    EXPECT_EQ(std::get<0>(r[1]), 2);
    EXPECT_EQ(std::get<0>(r[2]), 3);
}

TEST_F(StatementsV1, selectStatementsV1Where)
{
    std::string table = "sample";

    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Integer());

    ASSERT_NO_THROW(makeCreateTableStatement2(db, table, fldId, fldName, fldValue).execute());

    auto insertStatement = sqlite::makeInsertStatement(fldId, fldName, fldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, table));

    ASSERT_NO_THROW(insertStatement.insert(1, std::string{"A"}, 1));
    ASSERT_NO_THROW(insertStatement.insert(1, std::string{"B"}, 2));
    ASSERT_NO_THROW(insertStatement.insert(2, std::string{"C"}, 1));
    ASSERT_NO_THROW(insertStatement.insert(2, std::string{"D"}, 2));

    SelectStatement<decltype(fldId)> selectStatement(fldId);
    selectStatement.attach(db, table);

    int count = 0;
    auto countFuction = [&count](int id) {
        ++count;
        return true;
    };

    // reuse another where
    Where<decltype(fldId)> where1(selectStatement.getStatement(), op::eq(fldId));
    Where<decltype(fldId), decltype(fldValue)> where2(selectStatement.getStatement(),
                                                      op::and_(op::eq(fldId), op::eq(fldValue)));

    ASSERT_NO_THROW(where1.attach(selectStatement.getStatement(), op::eq(fldId)));
    ASSERT_NO_THROW(where2.attach(selectStatement.getStatement(), op::and_(op::eq(fldId), op::eq(fldValue))));

    count = 0;
    ASSERT_NO_THROW(selectStatement.where());
    ASSERT_NO_THROW(selectStatement.prepare());
    ASSERT_NO_THROW(selectStatement.exec(countFuction));
    ASSERT_EQ(count, 4);

    count = 0;
    ASSERT_NO_THROW(selectStatement.where(where1));
    ASSERT_NO_THROW(selectStatement.prepare());
    ASSERT_NO_THROW(where1.bind(1));
    ASSERT_NO_THROW(selectStatement.exec(countFuction));
    ASSERT_EQ(count, 2);

    count = 0;
    ASSERT_NO_THROW(selectStatement.where(where2));
    ASSERT_NO_THROW(selectStatement.prepare());
    ASSERT_NO_THROW(where2.bind(1, 1));
    ASSERT_NO_THROW(selectStatement.exec(countFuction));
    ASSERT_EQ(count, 1);

    count = 0;
    ASSERT_NO_THROW(selectStatement.where());
    ASSERT_NO_THROW(selectStatement.prepare());
    ASSERT_NO_THROW(selectStatement.exec(countFuction));
    ASSERT_EQ(count, 4);

    count = 0;
    ASSERT_NO_THROW(selectStatement.where(where1));
    ASSERT_NO_THROW(selectStatement.prepare());
    ASSERT_NO_THROW(where1.bind(2));
    ASSERT_NO_THROW(selectStatement.exec(countFuction));
    ASSERT_EQ(count, 2);
}

TEST_F(StatementsV1, casts)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, vint INTEGER, name TEXT, vreal DOUBLE)");
        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    long long lmax = std::numeric_limits<long long>::max();
    double dmax = std::numeric_limits<double>::max();
    // long
    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?,?)");

        ASSERT_NO_THROW(stmt.bind(1, 1));
        ASSERT_NO_THROW(stmt.bind(2, lmax));
        ASSERT_NO_THROW(stmt.bind(3, std::string{"name_1"}));
        ASSERT_NO_THROW(stmt.bind(4, dmax));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "SELECT vint, vreal FROM sample WHERE id == ?");

        stmt.bind(1, 1);

        long long lm = 0;
        double dm = 0;
        ASSERT_NO_THROW(stmt.execute([&stmt, &lm, &dm]() {
            lm = stmt.getLongValue(0);
            dm = stmt.getDoubleValue(1);
            return true;
        }));

        ASSERT_EQ(lm, lmax);
        ASSERT_EQ(dm, dmax);
    }

}

TEST_F(StatementsV1, move)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, v DOUBLE)");

        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    SQLiteStatement s;
    s = std::move(SQLiteStatement(db, "INSERT INTO sample VALUES (?,?,?)"));
}

TEST_F(StatementsV1, createWithStatementsV1)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, value DOUBLE)");
        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, statements::Insert("sample", std::make_tuple(fldId, fldName, fldValue)));
        ASSERT_NO_THROW(stmt.bind(std::make_tuple(0, "first name", -1.1)));
        ASSERT_NO_THROW(stmt.execute());
        ASSERT_NO_THROW(stmt.bind(std::make_tuple(1, "second name", 1.1)));
        ASSERT_NO_THROW(stmt.execute());
        ASSERT_NO_THROW(stmt.bind(std::make_tuple(2, "Third name", 3.1)));
        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, sqlite::statements::Select("sample", fldId, fldName, fldValue));

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count, &stmt]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 3);
    }

    {
        sqlite::statements::Select select("sample", fldId, fldName, fldValue);
        select.where(sqlite::op::eq(fldId));
        SQLiteStatement stmt(db, select);

        stmt.bind(std::make_tuple(1));
        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count, &stmt]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 1);
    }
}

TEST_F(StatementsV1, failAndRepeat)
{
    // Check that a failing statement is correctly reset after
    SQLiteStatement create(db, "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT)");
    ASSERT_NO_THROW(create.execute());

    SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?, ?);");
    ASSERT_NO_THROW(stmt.bind(std::make_tuple(0, "This ok")));
    ASSERT_NO_THROW(stmt.execute());
    ASSERT_NO_THROW(stmt.bind(std::make_tuple(0, "This Fails (Unique)")));
    ASSERT_THROW(stmt.execute(), sqlite::SQLiteException);
    ASSERT_NO_THROW(stmt.bind(std::make_tuple(1, "This Ok too")));
    ASSERT_NO_THROW(stmt.execute());

    ASSERT_NO_THROW(stmt.bind(std::make_tuple(1, "This Fails again (Unique)")));
    ASSERT_THROW(stmt.execute([]() { return true; }), sqlite::SQLiteException);
    ASSERT_NO_THROW(stmt.bind(std::make_tuple(2, "This Ok too")));
    ASSERT_NO_THROW(stmt.execute([]() { return true; }));

}

class SelectStatementsV1 : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    const FieldDef<FieldType::Integer> fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    const FieldDef<FieldType::Text> fldName = sqlite::makeFieldDef("n", sqlite::FieldType::Text());
    const FieldDef<FieldType::Real> fldValue = sqlite::makeFieldDef("v", sqlite::FieldType::Real());

    SelectStatementsV1()
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();

        SQLiteStatement create_stmt(db, "CREATE TABLE ex (id INTEGER, n TEXT, v DOUBLE)");
        create_stmt.executeStep();

        SQLiteStatement stmt(db, statements::Insert("ex", std::make_tuple(fldId, fldName, fldValue)));
        stmt.bind(std::make_tuple(1, "name1", 2.0));
        stmt.execute();
        stmt.bind(std::make_tuple(2, "name2", 4.0));
        stmt.execute();
        stmt.bind(std::make_tuple(3, "name3", 6.0));
        stmt.execute();
    }
};

TEST_F(SelectStatementsV1, execute)
{
    SQLiteStatement select(db, sqlite::statements::Select("ex", fldId));
    int count = 0;
    select.execute([&count]() {
        ++count;
        return true;
    });

    ASSERT_EQ(count, 3);
}

TEST(SelectStatementsV1WithDef, selectWithDefaultValues)
{
    FieldDef<FieldType::Integer> fldIdDef {"id", -1};
    FieldDef<FieldType::Text> fldNameDef {"n", "(null)"};
    FieldDef<FieldType::Real> fldValueDef{"v", -1.5};

    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->open();

    SQLiteStatement create_stmt(db, "CREATE TABLE ex (id INTEGER, n TEXT, v DOUBLE)");
    create_stmt.executeStep();;

    InsertStatement<decltype(fldIdDef), decltype(fldNameDef), decltype(fldValueDef)> in(fldIdDef, fldNameDef, fldValueDef);
    in.attach(db, "ex");
    in.prepare();

    in.insert();

    SelectStatement<decltype(fldIdDef), decltype(fldNameDef), decltype(fldValueDef)> sel(fldIdDef, fldNameDef, fldValueDef);
    sel.attach(db, "ex");
    sel.prepare();

    int id;
    std::string name;
    double value;
    sel.exec([&id, &name, &value](int i, std::string n, double v) {
        id = i;
        name = n;
        value = v;
        return true;
    });

    EXPECT_EQ(id, -1);
    EXPECT_EQ(name, "(null)");
    EXPECT_EQ(value, -1.5);
}


TEST_F(SelectStatementsV1, join)
{
    FieldDef<FieldType::Integer> jfldId = sqlite::makeFieldDef("jid", sqlite::FieldType::Integer());
    FieldDef<FieldType::Text> jfldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    FieldDef<FieldType::Real> jfldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    SQLiteStatement create_stmt(db, "CREATE TABLE ex2 (jid INTEGER, name TEXT, value DOUBLE)");
    create_stmt.execute();

    SQLiteStatement stmt(db, statements::Insert("ex2", std::make_tuple(jfldId, jfldName, jfldValue)));
    stmt.bind(std::make_tuple(1, "a", -2.0));
    stmt.execute();
    stmt.bind(std::make_tuple(2, "b", -4.0));
    stmt.execute();
    stmt.bind(std::make_tuple(3, "c", -6.0));
    stmt.execute();

    {
        auto s = sqlite::statements::Select("ex", fldId, jfldName, fldValue, jfldValue);
        s.join("ex2", fldId, field("ex2", jfldId));

        std::vector<int> ids;
        std::vector<std::string> names;
        SQLiteStatement stmt(db, s);
        ASSERT_NO_THROW(stmt.execute([&ids, &names, &stmt]() {
            ids.push_back(stmt.getIntValue(0));
            names.push_back(stmt.getStringValue(1));
            return true;
        }));

        ASSERT_EQ(ids.size(), 3);
        ASSERT_EQ(ids[0], 1);
        ASSERT_EQ(names[0], "a");
        ASSERT_EQ(names[1], "b");
        ASSERT_EQ(names[2], "c");
    }
}

class DeleteStatementsV1 : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    const FieldDef<FieldType::Integer> fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    const FieldDef<FieldType::Text> fldName = sqlite::makeFieldDef("n", sqlite::FieldType::Text());
    const FieldDef<FieldType::Real> fldValue = sqlite::makeFieldDef("v", sqlite::FieldType::Real());

    int count()
    {
        int c = 0;
        SQLiteStatement select(db, "SELECT id FROM ex;");
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

        SQLiteStatement create_stmt(db, "CREATE TABLE ex (id INTEGER, n TEXT, v DOUBLE);");
        create_stmt.executeStep();

        SQLiteStatement stmt(db, statements::Insert("ex", std::make_tuple(fldId, fldName, fldValue)));
        stmt.bind(std::make_tuple(1, "name1", 2.0));
        stmt.execute();
        stmt.bind(std::make_tuple(2, "name2", 4.0));
        stmt.execute();
        stmt.bind(std::make_tuple(3, "name3", 6.0));
        stmt.execute();

        Test::SetUp();
    }
};

TEST_F(DeleteStatementsV1, exec)
{
    // first check that everything is properly prepared.
    ASSERT_EQ(count(), 3);

    DeleteStatement deleteStatement;
    ASSERT_NO_THROW(deleteStatement.attach(db, "ex"));

    Where<decltype(fldId)> whereClause(deleteStatement.getStatement(), op::eq(fldId));
    ASSERT_NO_THROW(deleteStatement.where(whereClause));
    ASSERT_NO_THROW(deleteStatement.prepare());
    ASSERT_NO_THROW(whereClause.bind(1));
    ASSERT_NO_THROW(deleteStatement.exec());
    ASSERT_EQ(count(), 2);

    DeleteStatement deleteAllStatement;
    ASSERT_NO_THROW(deleteAllStatement.attach(db, "ex"));
    ASSERT_NO_THROW(deleteAllStatement.prepare());
    ASSERT_NO_THROW(deleteAllStatement.exec());
    ASSERT_EQ(count(), 0);
}

TEST_F(DeleteStatementsV1, whereReset)
{
    // first check that everything is properly prepared.
    ASSERT_EQ(count(), 3);

    DeleteStatement deleteStatement;
    ASSERT_NO_THROW(deleteStatement.attach(db, "ex"));

    Where<decltype(fldId)> whereClause(deleteStatement.getStatement(), op::eq(fldId));
    ASSERT_NO_THROW(deleteStatement.where(whereClause));
    ASSERT_NO_THROW(deleteStatement.prepare());
    ASSERT_NO_THROW(whereClause.bind(1));
    ASSERT_NO_THROW(deleteStatement.exec());
    ASSERT_EQ(count(), 2);

    // remove the where

    ASSERT_NO_THROW(deleteStatement.where());
    ASSERT_NO_THROW(deleteStatement.prepare());
    ASSERT_NO_THROW(deleteStatement.exec());
    ASSERT_EQ(count(), 0);

    // Use another where statement

    Where<decltype(fldName)> whereClause2(deleteStatement.getStatement(), op::eq(fldName));
    ASSERT_NO_THROW(deleteStatement.where(whereClause2));
    ASSERT_NO_THROW(deleteStatement.prepare());
    ASSERT_NO_THROW(whereClause2.bind("first"));
    ASSERT_NO_THROW(deleteStatement.exec());
}