#include "sqlitestorage.h"
#include "sqlitetable.h"
#include "sqlitefielddef.h"

#include <iostream>

using namespace sqlite;
using namespace std;

int main()
{
    auto db = std::make_shared<SQLiteStorage>("output.db");
    db->open();

    auto testTable = std::make_tuple(
        makeFieldDef("Id", FieldType::Integer()), //FieldAttribute::AutoIncrement),
        makeFieldDef("name", FieldType::Text()),
        makeFieldDef("x", FieldType::Real()),
        makeFieldDef("y", FieldType::Real())
    );

    cout << "Build String: \n" << sqlite::buildSqlCreateString(testTable);

    try {
        db->dropTable("sample");
    } catch (sqlite::SQLiteException &) { // ignore errors
    }

    auto table = SQLiteTable::create(db, "sample", testTable);

    struct Record {
        int id;
        std::string name;
        double x,y;
    };

    Record record = {
        0, "first", 1.16f, -1.98f
    };

    table.insert (testTable, std::make_tuple(record.id, record.name, record.x, record.y));

    Record r2 = { 1, "uno", 0, 1000};
    table.insert (testTable, std::make_tuple(r2.id, r2.name, r2.x, r2.y));
}
