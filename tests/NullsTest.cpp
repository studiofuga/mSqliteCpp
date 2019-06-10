/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 15/06/18
 */

#include "msqlitecpp/insertstatement.h"
#include "msqlitecpp/createstatement.h"
#include "msqlitecpp/selectstatement.h"

#include <gtest/gtest.h>

using namespace sqlite;

class NullsTest : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    NullsTest()
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();
    }

    FieldDef<FieldType::Integer> fieldId{"id", PrimaryKey};
    FieldDef<FieldType::Text> fieldText{"text"};
    FieldDef<FieldType::Integer> fieldCount{"count"};
    FieldDef<FieldType::Real> fieldValue{"value"};
};


TEST_F(NullsTest, insert)
{
    auto createTable = makeCreateTableStatement2(db, "Insert1", fieldId, fieldText, fieldCount, fieldValue);
    ASSERT_NO_THROW(createTable.execute());

    auto insertStatement_2 = makeInsertStatement(fieldId, fieldText);
    insertStatement_2.replaceOnConflict();

    ASSERT_NO_THROW(insertStatement_2.attach(db, "Insert1"));
    ASSERT_NO_THROW(insertStatement_2.insert(1, "Sample"));

    auto select = makeSelectStatement(fieldId, fieldText, fieldCount, fieldValue);
    select.attach(db, "Insert1");
    select.prepare();

    // check insert

    int rId, rCount;
    std::string rText;
    double rValue;
    select.exec([&rId, &rText, &rCount, &rValue](int id, std::string text, int count, double value) {
        rId = id;
        rText = text;
        rCount = count;
        rValue = value;
        return true;
    });

    ASSERT_EQ(rId, 1);
    ASSERT_EQ(rText, "Sample");
    ASSERT_EQ(rCount, 0);
    ASSERT_EQ(rValue, 0);

    auto insertStatement_3 = makeInsertStatement(fieldId, fieldText, fieldCount, fieldValue);
    insertStatement_3.replaceOnConflict();

    ASSERT_NO_THROW(insertStatement_3.attach(db, "Insert1"));
    ASSERT_NO_THROW(insertStatement_3.insert(1, nullptr, 10, -4));

    select.exec([&rId, &rText, &rCount, &rValue](int id, std::string text, int count, double value) {
        rId = id;
        rText = text;
        rCount = count;
        rValue = value;
        return true;
    });

    ASSERT_EQ(rId, 1);
    ASSERT_EQ(rText, "");
    ASSERT_EQ(rCount, 10);
    ASSERT_EQ(rValue, -4);


    ASSERT_NO_THROW(insertStatement_3.insert(1, nullptr, nullptr, nullptr));

    select.exec([&rId, &rText, &rCount, &rValue](int id, std::string text, int count, double value) {
        rId = id;
        rText = text;
        rCount = count;
        rValue = value;
        return true;
    });

    ASSERT_EQ(rId, 1);
    ASSERT_EQ(rText, "");
    ASSERT_EQ(rCount, 0);
    ASSERT_EQ(rValue, 0);
}

#if defined(WITH_BOOST)
TEST_F(NullsTest, selectWithNulls)
{
    auto createTable = makeCreateTableStatement2(db, "Insert1", fieldId, fieldText, fieldCount, fieldValue);
    ASSERT_NO_THROW(createTable.execute());

    auto insertStatement_2 = makeInsertStatement(fieldId, fieldText);
    insertStatement_2.replaceOnConflict();

    ASSERT_NO_THROW(insertStatement_2.attach(db, "Insert1"));
    ASSERT_NO_THROW(insertStatement_2.insert(1, "Sample"));

    auto select = makeSelectStatement(fieldId, fieldText, fieldCount, fieldValue);
    select.attach(db, "Insert1");
    select.prepare();

    boost::optional<int> rId, rCount;
    boost::optional<std::string> rText;
    boost::optional<double> rValue;
    select.execOpt([&rId, &rText, &rCount, &rValue](boost::optional<int> id, boost::optional<std::string> text,
                                                 boost::optional<int> count, boost::optional<double> value) {
        rId = id;
        rText = text;
        rCount = count;
        rValue = value;
        return true;
    });

    ASSERT_TRUE(rId.is_initialized());
    ASSERT_TRUE(rText.is_initialized());
    ASSERT_FALSE(rCount.is_initialized());
    ASSERT_FALSE(rValue.is_initialized());

    ASSERT_EQ(rId.value(), 1);
    ASSERT_EQ(rText.value(), "Sample");
}
#endif
