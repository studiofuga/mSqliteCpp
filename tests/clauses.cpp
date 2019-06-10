//
// Created by Federico Fuga on 06/04/18.
//

#include <gtest/gtest.h>
#include <msqlitecpp/sqlitestatement.h>

#include "msqlitecpp/sqlitefielddef.h"
#include "msqlitecpp/clauses.h"

using namespace sqlite;

class Clauses : public ::testing::Test {
    std::shared_ptr<SQLiteStorage> db;
public:
    const FieldDef<FieldType::Integer> fId;
    const FieldDef<FieldType::Text> fName;
    const FieldDef<FieldType::Real> fValue;

    Clauses()
            : fId("id"), fName("name"), fValue("value") {

    }

protected:
    void SetUp() override
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        ASSERT_NO_THROW(db->open());

        SQLiteStatement insert(db, "CREATE TABLE sample (id INTEGER PRIMARY KEY, name TEXT, value REAL);");
        insert.execute();
    }


};

TEST_F(Clauses, Where)
{
    /*
    Where<decltype(fId)> where(fId);

    ASSERT_EQ(where.toText(), "WHERE id = ?");*/
}