/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 18/09/18
 */

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

TEST(V1ForeignKeys, Enforce)
{
    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->setFlag(sqlite::SQLiteStorage::Flags::EnforceForeignKeys);
    db->open();

    sqlite::FieldDef<sqlite::FieldType::Integer> fldTheId{"id", Unique};
    sqlite::FieldDef<sqlite::FieldType::Integer> fldRef{"ref"};
    CreateTableStatement<decltype(fldTheId), decltype(fldRef)> createFirst(db, "First",
                                                                           fldTheId, fldRef);
    ASSERT_NO_THROW(createFirst.execute());

    sqlite::FieldDef<sqlite::FieldType::Integer> fldId{"sid", Unique};
    sqlite::FieldDef<sqlite::FieldType::Text> fldName{"name"};
    sqlite::FieldDef<sqlite::FieldType::Integer> fldValue{"fid"};
    CreateTableStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> createSecond(db, "Second", fldId, fldName,
                                                                                                fldValue);

    statements::CreateTable::TableConstraint::ForeignKey fk("fk",
                                                            std::make_tuple(fldValue),
                                                            "First",
                                                            std::make_tuple(fldTheId));

    fk.onUpdate(statements::CreateTable::TableConstraint::ForeignKey::Action::Cascade);
    fk.onDelete(statements::CreateTable::TableConstraint::ForeignKey::Action::Cascade);

    createSecond.setTableConstraint(fk);

    ASSERT_NO_THROW(createSecond.execute());

    // First Insert First
    sqlite::InsertStatement<decltype(fldTheId), decltype(fldRef)> fInsert(fldTheId, fldRef);
    fInsert.attach(db, "First");

    ASSERT_NO_THROW(fInsert.insert(1, 1));

    sqlite::InsertStatement<decltype(fldId), decltype(fldName), decltype(fldValue)> sInsert(fldId, fldName, fldValue);
    sInsert.attach(db, "Second");

    ASSERT_NO_THROW(sInsert.insert(1, "One", 1));
    ASSERT_THROW(sInsert.insert(2, "Two", 12983), sqlite::SQLiteException);
    ASSERT_NO_THROW(sInsert.insert(3, "Three", 1));
}
