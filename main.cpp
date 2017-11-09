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

    auto table = SQLiteTable::create(db, "sample", testTable);

#if 0
    struct Record {
        int id;
        std::string name;
        float x,y;
    };

    Record record = {
        0, "first", 1.1, -1.98
    };

    table.insert (testTable, std::make_tuple(record.id, record.name, record.x, record.y));
#endif
}
