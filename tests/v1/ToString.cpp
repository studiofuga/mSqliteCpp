//
// Created by happycactus on 14/10/20.
//

#include "msqlitecpp/v1/sqlitestatementformatters.h"
#include "msqlitecpp/v1/createstatement.h"

#include <gtest/gtest.h>

using namespace sqlite;

struct FieldInt1 : public FieldDef<FieldType::Integer> {
    FieldInt1() : FieldDef<FieldType::Integer>("f1")
    {}
};

struct FieldInt2 : public FieldDef<FieldType::Integer> {
    FieldInt2() : FieldDef<FieldType::Integer>("f2")
    {}
};

TEST(V1ToString, DerivedClass)
{
    FieldInt1 t1;
    FieldDef<FieldType::Integer> e1{"f1"};

    EXPECT_EQ(sqlite::statements::details::toString(t1), sqlite::statements::details::toString(e1));

    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->open();

    CreateTableStatement<FieldInt1, FieldInt2> c{db, "x", FieldInt1{}, FieldInt2{}};

    EXPECT_EQ(c.statementString(), "CREATE TABLE x (f1 INTEGER, f2 INTEGER)");
}
