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
    db->open();

    db->tableExists("do-not-exists");
}