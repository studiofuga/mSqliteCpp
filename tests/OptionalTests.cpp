/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 15/06/18
 */

#include "createstatement.h"
#include "insertstatement.h"
#include "selectstatement.h"

#include <boost/optional.hpp>

#include <gtest/gtest.h>
#include <clauses.h>
#include <sqlitefieldsop.h>

using namespace sqlite;

class OptionalStatements : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    OptionalStatements()
    {
        //db = std::make_shared<SQLiteStorage>("OptionalStatement.db");
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();
    }

    FieldDef<FieldType::Integer> fieldId{"id", PrimaryKey};
    FieldDef<FieldType::Text> fieldText{"text"};
    FieldDef<FieldType::Integer> fieldCount{"count"};
    FieldDef<FieldType::Real> fieldValue{"value"};
};

TEST_F(OptionalStatements, FieldsFormat)
{
    boost::optional<FieldDef<FieldType::Text>> text;
    boost::optional<FieldDef<FieldType::Integer>> count;
    boost::optional<FieldDef<FieldType::Real>> value;

    ASSERT_EQ(statements::details::toString(text), "");
    ASSERT_EQ(statements::details::toString(count), "");
    ASSERT_EQ(statements::details::toString(value), "");

    text = fieldText;

    ASSERT_EQ(statements::details::toString(text), "text");
}

TEST_F(OptionalStatements, Unpack)
{
    boost::optional<FieldDef<FieldType::Text>> text;
    boost::optional<FieldDef<FieldType::Integer>> count;
    boost::optional<FieldDef<FieldType::Real>> value;

    value = fieldValue;

    std::string fields = sqlite::statements::unpackFieldNames(fieldId, text, value);
    ASSERT_EQ(fields, "id,value");
    fields = sqlite::statements::unpackFieldNames(fieldId);
    ASSERT_EQ(fields, "id");
    fields = sqlite::statements::unpackFieldNames(text);
    ASSERT_EQ(fields, "");
    fields = sqlite::statements::unpackFieldNames(value);
    ASSERT_EQ(fields, "value");

    auto placeholders = sqlite::statements::unpackFieldPlaceholders(fieldId, text, value);
    ASSERT_EQ(placeholders, "?,?");
    placeholders = sqlite::statements::unpackFieldPlaceholders(fieldId);
    ASSERT_EQ(placeholders, "?");
    placeholders = sqlite::statements::unpackFieldPlaceholders(text);
    ASSERT_EQ(placeholders, "");
    placeholders = sqlite::statements::unpackFieldPlaceholders(value);
    ASSERT_EQ(placeholders, "?");

    auto fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholders(fieldId, text, value);
    ASSERT_EQ(fieldplaceholders, "id = ?,value = ?");

    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholders(fieldId);
    ASSERT_EQ(fieldplaceholders, "id = ?");

    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholders(text);
    ASSERT_EQ(fieldplaceholders, "");

    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholders(value);
    ASSERT_EQ(fieldplaceholders, "value = ?");
}

TEST_F(OptionalStatements, UnpackOptionals)
{
    boost::optional<int> vid{1};
    boost::optional<std::string> vtext;
    boost::optional<double> vvalue{10.0};

    std::string fields = sqlite::statements::unpackFieldNamesOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));

    auto placeholders = sqlite::statements::unpackFieldPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    auto fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    ASSERT_EQ(fields, "id,value");
    ASSERT_EQ(placeholders, "?,?");
    ASSERT_EQ(fieldplaceholders, "id = ?,value = ?");

    vvalue.reset();
    fields = sqlite::statements::unpackFieldNamesOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    placeholders = sqlite::statements::unpackFieldPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    ASSERT_EQ(fields, "id");
    ASSERT_EQ(placeholders, "?");
    ASSERT_EQ(fieldplaceholders, "id = ?");

    vid.reset();
    fields = sqlite::statements::unpackFieldNamesOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    placeholders = sqlite::statements::unpackFieldPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    ASSERT_EQ(fields, "");
    ASSERT_EQ(placeholders, "");
    ASSERT_EQ(fieldplaceholders, "");

    vvalue = 0.1;
    fields = sqlite::statements::unpackFieldNamesOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    placeholders = sqlite::statements::unpackFieldPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    fieldplaceholders = sqlite::statements::unpackFieldsAndPlaceholdersOpt(
            std::make_tuple(fieldId, fieldText, fieldValue),
            std::make_tuple(vid, vtext, vvalue));
    ASSERT_EQ(fields, "value");
    ASSERT_EQ(placeholders, "?");
    ASSERT_EQ(fieldplaceholders, "value = ?");
}

TEST_F(OptionalStatements, InsertFormatter)
{
    boost::optional<FieldDef<FieldType::Text>> text;
    boost::optional<FieldDef<FieldType::Integer>> count;
    boost::optional<FieldDef<FieldType::Real>> value;

    text = fieldText;

    ASSERT_EQ(statements::details::toString(text), "text");

    sqlite::statements::Insert
            insert("tab", std::make_tuple(text, count, value));

    std::string exp{
            "INSERT INTO tab(text) VALUES(?);"
    };

    ASSERT_EQ(exp, insert.string());
}

TEST_F(OptionalStatements, Insert)
{
    auto createTable = makeCreateTableStatement2(db, "Insert1", fieldId, fieldText, fieldCount, fieldValue);
    ASSERT_NO_THROW(createTable.execute());

    boost::optional<std::string> text{"sampleText"};
    boost::optional<int> count{1};
    boost::optional<double> value{0.1};

    auto insertStatement = makeInsertStatement(fieldId, fieldText, fieldCount, fieldValue);
    insertStatement.replaceOnConflict();

    ASSERT_NO_THROW(insertStatement.attach(db, "Insert1"));
    ASSERT_NO_THROW(insertStatement.insert(1, text, count, value));

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
    ASSERT_EQ(rText, "sampleText");
    ASSERT_EQ(rCount, 1);
    ASSERT_EQ(rValue, 0.1);

    // ** now update

    text = "NewText";
    count.reset();
    value.reset();

    ASSERT_NO_THROW(insertStatement.prepareAndInsert(1, text, count, value));

    select.exec([&rId, &rText, &rCount, &rValue](int id, std::string text, int count, double value) {
        rId = id;
        rText = text;
        rCount = count;
        rValue = value;
        return true;
    });

    ASSERT_EQ(rId, 1);
    ASSERT_EQ(rText, "NewText");
    ASSERT_EQ(rCount, 1);
    ASSERT_EQ(rValue, 0.1);

// update again...
    // ** now update

    text.reset();
    count.reset();
    value = 1000;

    ASSERT_NO_THROW(insertStatement.insert(1, text, count, value));

    select.exec([&rId, &rText, &rCount, &rValue](int id, std::string text, int count, double value) {
        rId = id;
        rText = text;
        rCount = count;
        rValue = value;
        return true;
    });

    ASSERT_EQ(rId, 1);
    ASSERT_EQ(rText, "NewText");
    ASSERT_EQ(rCount, 1);
    ASSERT_EQ(rValue, 1000);

}

TEST_F(OptionalStatements, Issue6InsertMultipleOptionals)
{
    auto createTable = makeCreateTableStatement2(db, "Insert1", fieldId, fieldText, fieldCount, fieldValue);
    ASSERT_NO_THROW(createTable.execute());

    boost::optional<std::string> text{"sampleText"};
    boost::optional<int> countvalue{1};
    boost::optional<double> value{0.1};

    auto insertStatement = makeInsertStatement(fieldId, fieldText, fieldCount, fieldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, "Insert1"));

    auto select = makeSelectStatement(fieldId, fieldText, fieldCount, fieldValue);
    select.attach(db, "Insert1");
    select.prepare();

    int count = 0;
    auto countFunction = [&count](int id, std::string, int, double) {
        ++count;
        return true;
    };

    // insert one
    ASSERT_NO_THROW(insertStatement.insert(1, text, countvalue, value));

    count = 0;
    select.exec(countFunction);
    ASSERT_EQ(count, 1);

    // insert another. It should have a null text, countvalue and value
    text.reset();
    countvalue.reset();
    value.reset();
    ASSERT_NO_THROW(insertStatement.insert(2, text, countvalue, value));

    count = 0;
    select.exec(countFunction);
    ASSERT_EQ(count, 2);

    Where<decltype(fieldId)> w;
    w.attach(select.getStatement(), op::eq(fieldId));
    select.where(w);
    select.prepare();

    std::string rtext;
    int rcountvalue;
    double rvalue;
    w.bind(2);
    select.exec([&rtext, &rvalue, &rcountvalue](int id, std::string t, int c, double v) {
        rtext = t;
        rcountvalue = c;
        rvalue = v;
        return true;
    });

    ASSERT_EQ(rtext, "");
    ASSERT_EQ(rcountvalue, 0);
    ASSERT_EQ(rvalue, 0);
}

