/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 11/06/19
 */

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"
#include "msqlitecpp/v2/helpers/formatters.h"
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

    template<typename ...T>
    int count(SelectStatement<T...> statement)
    {
        size_t count = 0;
        statement.execute([&count](int i1, int i2, std::string t) {
            ++count;
            return true;
        });
        return count;
    }
};

char const *SelectStatements::tableName = "t";

TEST_F(SelectStatements, simpleSelect)
{
    auto select = makeSelectStatement(db, tableName, int1, int2, text);

    size_t countN = count(select);
    ASSERT_EQ(countN, 5);
}

TEST_F(SelectStatements, selectWithWhere)
{
    auto select = makeSelectStatement(db, tableName, int1, int2, text);

    auto whereClause = (int1 == "x");

    ASSERT_EQ(whereClause.t, "field1 = :x");

    ASSERT_NO_THROW(select.where(whereClause));
    ASSERT_NO_THROW(select.bind(1));
    ASSERT_EQ(count(select), 2);
}

TEST_F(SelectStatements, selectWithWhere2)
{
    auto select = makeSelectStatement(db, tableName, int1, int2, text);

    auto whereClause = (int1 == "x" && int2 == "y");

    ASSERT_EQ(whereClause.t, "field1 = :x AND field2 = :y");

    ASSERT_NO_THROW(select.where(whereClause));
    ASSERT_NO_THROW(select.bind(1, 1));
    ASSERT_EQ(count(select), 1);
}

TEST_F(SelectStatements, whereReuse)
{
    auto select = makeSelectStatement(db, tableName, int1, int2, text);

    auto whereClause = (int1 == "x") && (int2 == "y");

    ASSERT_NO_THROW(select.where(whereClause));
    ASSERT_NO_THROW(select.bind(1, 1));
    ASSERT_EQ(count(select), 1);

    ASSERT_NO_THROW(select.where(int1 == "x"));
    ASSERT_NO_THROW(select.bind(1));
    ASSERT_EQ(count(select), 2);

    ASSERT_NO_THROW(select.where());
    ASSERT_EQ(count(select), 5);
}
