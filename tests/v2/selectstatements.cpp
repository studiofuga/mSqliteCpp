/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 11/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/createstatement.h"
#include "msqlitecpp/v2/insertstatement.h"
#include "msqlitecpp/v2/selectstatement.h"

#include <gtest/gtest.h>

using namespace msqlitecpp::v2;

class SelectStatements : public testing::Test {
protected:
    static char const *tableName;

    Storage db;

    Column<ColumnTypes::Integer> int1{"field1"};
    Column<ColumnTypes::Integer> int2{"field2"};
    Column<ColumnTypes::Text> text{"textfield"};

public:
    SelectStatements()
            : db(Storage::inMemory())
    {
        makeCreateStatement(db, tableName, int1, int2, text).execute();

        auto insertStatement = makeInsertStatement(db, tableName, int1, int2, text);

        insertStatement.insert(1, 1, "1-1");
        insertStatement.insert(1, 2, "1-2");
        insertStatement.insert(2, 1, "2-1");
        insertStatement.insert(2, 2, "2-2");
        insertStatement.insert(3, 0, "3-0");
    }
};

char const *SelectStatements::tableName = "t";

TEST_F(SelectStatements, simpleSelect)
{
    auto select = makeSelectStatement(db, tableName, int1, int2, text);

    size_t count = 0;
    select.execute([&count](int i1, int i2, std::string t) {
        ++count;
        return true;
    });

    ASSERT_EQ(count, 5);
}