//
// Created by Federico Fuga on 19/12/17.
//

#include "insertstatement.h"
#include "deletestatement.h"
#include "sqlitestorage.h"
#include "sqlitestatement.h"
#include "selectstatement.h"
#include "createstatement.h"

#include "sqlitestatementformatters.h"
#include "sqlitefieldsop.h"
#include "clauses.h"

#include "gtest/gtest.h"

using namespace sqlite;

class Statements : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    Statements()
            : fldId("id", sqlite::FieldAttribute::NotNull),
              fldName("name", sqlite::FieldAttribute::NotNull),
              fldValue("v", sqlite::FieldAttribute::NotNull)
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();
    }

    sqlite::FieldDef<sqlite::FieldType::Integer> fldId;
    sqlite::FieldDef<sqlite::FieldType::Text> fldName;
    sqlite::FieldDef<sqlite::FieldType::Real> fldValue;
};

TEST_F(Statements, CreateTable)
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
              "CREATE TABLE other (id2 INTEGER, text TEXT, CONSTRAINT u FOREIGN KEY(id2) REFERENCES other(id));"
    );

}

TEST_F(Statements, CreateTableWithConstraint)
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

TEST_F(Statements, FixCreateTableInvertedAttachConstraint)
{
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create(fldId, fldName,
                                                                                        fldValue);

    statements::CreateTable::TableConstraint::Unique unique("u", fldId, fldName);

    create.attach(db, "sample");
    create.setTableConstraint(unique);

    ASSERT_EQ(create.statementString(),
              "CREATE TABLE sample (id INTEGER NOT NULL, name TEXT NOT NULL, v REAL NOT NULL, CONSTRAINT u UNIQUE (id,name));"
    );

    // here we invert the two functions

    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> create2(fldId, fldName,
                                                                                        fldValue);

    statements::CreateTable::TableConstraint::Unique unique2("u2", fldId, fldName);

    create2.setTableConstraint(unique2);
    create2.attach(db, "sample");

    ASSERT_EQ(create2.statementString(),
              "CREATE TABLE sample (id INTEGER NOT NULL, name TEXT NOT NULL, v REAL NOT NULL, CONSTRAINT u2 UNIQUE (id,name));"
    );
}


TEST_F(Statements, FixCreateTableMultipleConstraint)
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
              " CONSTRAINT u2 UNIQUE (v));"
    );
}

TEST_F(Statements, createIndex)
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
    ASSERT_EQ(createIndex2.statementString(), "CREATE UNIQUE INDEX unqIdx ON mytable(id,name);");
    ASSERT_NO_THROW(createIndex2.execute());
}

TEST_F(Statements, create)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, v DOUBLE);");

        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?);");

        ASSERT_NO_THROW(stmt.bind(1, 0));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name"}));
        ASSERT_NO_THROW(stmt.bind(3, 1.5));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?);");

        ASSERT_NO_THROW(stmt.bind(1, 1));
        ASSERT_NO_THROW(stmt.bind(2, std::string{"name_1"}));
        ASSERT_NO_THROW(stmt.bind(3, 100.2));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "SELECT id, name, v FROM sample;");

        int count = 0;
        ASSERT_NO_THROW(stmt.execute([&count]() {
            ++count;
            return true;
        }));

        ASSERT_EQ(count, 2);
    }
}

TEST_F(Statements, typedCreate)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text()).notNull();
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real()).notNull();

    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT NOT NULL, value DOUBLE NOT NULL);");
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

TEST_F(Statements, statementExecuteFail)
{
    SQLiteStatement stmt(db,
                         "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT);");
    ASSERT_NO_THROW(stmt.execute());

    stmt.attach(db,
                "INSERT INTO sample VALUES (0, 'AAA'),(1,'BBB'),(2,'CCC');");
    ASSERT_NO_THROW(stmt.execute());

    // fails at mid
    stmt.attach(db, "SELECT * from SAMPLE;");
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

TEST_F(Statements, insertFromQuery)
{
    SQLiteStatement stmt(db,
                         "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT);");
    ASSERT_NO_THROW(stmt.execute());

    stmt.attach(db,
                "INSERT INTO sample VALUES (0, 'AAA'),(1,'BBB'),(2,'CCC');");
    ASSERT_NO_THROW(stmt.execute());

    stmt.attach(db,
                "CREATE TABLE newsample (id INTEGER PRIMARY KEY, relid INTEGER, name TEXT);");
    ASSERT_NO_THROW(stmt.execute());


    stmt.attach(db,
                "INSERT INTO newsample (relid, name) SELECT  id,? FROM sample WHERE id=?");

    ASSERT_NO_THROW(stmt.bind(1, "aiuto"));
    ASSERT_NO_THROW(stmt.bind(2, 0));

    ASSERT_NO_THROW(stmt.execute());
}

TEST_F(Statements, selectStatements1)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer()).notNull();
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text()).notNull();
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real()).notNull();

    {
        SQLiteStatement stmt(db,
                             "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT NOT NULL, value DOUBLE NOT NULL);");
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
}

TEST_F(Statements, casts)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, vint INTEGER, name TEXT, vreal DOUBLE);");
        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    long long lmax = std::numeric_limits<long long>::max();
    double dmax = std::numeric_limits<double>::max();
    // long
    {
        SQLiteStatement stmt(db, "INSERT INTO sample VALUES (?,?,?,?);");

        ASSERT_NO_THROW(stmt.bind(1, 1));
        ASSERT_NO_THROW(stmt.bind(2, lmax));
        ASSERT_NO_THROW(stmt.bind(3, std::string{"name_1"}));
        ASSERT_NO_THROW(stmt.bind(4, dmax));

        ASSERT_NO_THROW(stmt.executeStep());
    }

    {
        SQLiteStatement stmt(db, "SELECT vint, vreal FROM sample WHERE id == ?;");

        stmt.bind(1, 1);

        long lm = 0;
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

TEST_F(Statements, move)
{
    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, v DOUBLE);");

        ASSERT_NO_THROW(stmt.executeStep([]() { return true; }));
    }

    SQLiteStatement s;
    s = std::move(SQLiteStatement(db, "INSERT INTO sample VALUES (?,?,?);"));
}

TEST_F(Statements, createWithStatements)
{
    auto fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    auto fldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    auto fldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    {
        SQLiteStatement stmt(db, "CREATE TABLE sample (id INTEGER, name TEXT, value DOUBLE);");
        ASSERT_NO_THROW(stmt.execute());
    }

    {
        SQLiteStatement stmt(db, statements::Insert("sample", fldId, fldName, fldValue));
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

TEST_F(Statements, failAndRepeat)
{
    // Check that a failing statement is correctly reset after
    SQLiteStatement create(db, "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT);");
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

class SelectStatements : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    const FieldDef<FieldType::Integer> fldId = sqlite::makeFieldDef("id", sqlite::FieldType::Integer());
    const FieldDef<FieldType::Text> fldName = sqlite::makeFieldDef("n", sqlite::FieldType::Text());
    const FieldDef<FieldType::Real> fldValue = sqlite::makeFieldDef("v", sqlite::FieldType::Real());

    SelectStatements()
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();

        SQLiteStatement create_stmt(db, "CREATE TABLE ex (id INTEGER, n TEXT, v DOUBLE);");
        create_stmt.executeStep();

        SQLiteStatement stmt(db, statements::Insert("ex", fldId, fldName, fldValue));
        stmt.bind(std::make_tuple(1, "name1", 2.0));
        stmt.execute();
        stmt.bind(std::make_tuple(2, "name2", 4.0));
        stmt.execute();
        stmt.bind(std::make_tuple(3, "name3", 6.0));
        stmt.execute();
    }
};

TEST_F(SelectStatements, execute)
{
    SQLiteStatement select(db, sqlite::statements::Select("ex", fldId));
    int count = 0;
    select.execute([&count]() {
        ++count;
        return true;
    });

    ASSERT_EQ(count, 3);
}

TEST_F(SelectStatements, join)
{
    FieldDef<FieldType::Integer> jfldId = sqlite::makeFieldDef("jid", sqlite::FieldType::Integer());
    FieldDef<FieldType::Text> jfldName = sqlite::makeFieldDef("name", sqlite::FieldType::Text());
    FieldDef<FieldType::Real> jfldValue = sqlite::makeFieldDef("value", sqlite::FieldType::Real());

    SQLiteStatement create_stmt(db, "CREATE TABLE ex2 (jid INTEGER, name TEXT, value DOUBLE);");
    create_stmt.execute();

    SQLiteStatement stmt(db, statements::Insert("ex2", jfldId, jfldName, jfldValue));
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

class DeleteStatements : public testing::Test {
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

        SQLiteStatement stmt(db, statements::Insert("ex", fldId, fldName, fldValue));
        stmt.bind(std::make_tuple(1, "name1", 2.0));
        stmt.execute();
        stmt.bind(std::make_tuple(2, "name2", 4.0));
        stmt.execute();
        stmt.bind(std::make_tuple(3, "name3", 6.0));
        stmt.execute();

        Test::SetUp();
    }
};

TEST_F(DeleteStatements, exec)
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