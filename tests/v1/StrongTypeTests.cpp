//
// Created by Federico Fuga on 18/06/18.
//

#include "msqlitecpp/v1/sqlitestatement.h"

template <typename T, typename TAG>
class StrongType {
private:
    T value;
public:
    StrongType (T&& v)
            : value(std::forward<T>(v)) {

    }

    T toValue() const
    {
        return value;
    }
};

namespace sqlite {

template <typename T, typename TAG>
inline void bind (SQLiteStatement &statement, size_t s, const StrongType<T,TAG> &strongType) {
    statement.bind(s, strongType.toValue());
}
}

#include "msqlitecpp/v1/insertstatement.h"
#include "msqlitecpp/v1/createstatement.h"
#include "msqlitecpp/v1/selectstatement.h"

#include <gtest/gtest.h>

using namespace sqlite;


class StrongTypeTest : public testing::Test {
protected:
    std::shared_ptr<SQLiteStorage> db;
public:
    StrongTypeTest()
    {
        db = std::make_shared<SQLiteStorage>(":memory:");
        db->open();

        auto createTable = makeCreateTableStatement2(db, "Insert1", fieldId, fieldText, fieldCount, fieldValue);
        createTable.execute();
    }

    FieldDef<FieldType::Integer> fieldId{"id", PrimaryKey};
    FieldDef<FieldType::Text> fieldText {"text"};
    FieldDef<FieldType::Integer> fieldCount {"count"};
    FieldDef<FieldType::Real> fieldValue {"value"};
};

TEST_F(StrongTypeTest, insertStrongTypes)
{
    struct MyIntTag{};
    struct MyStringTag{};
    struct MyDoubleTag{};

    using MyInt = StrongType<int, MyIntTag>;
    using MyString = StrongType<std::string, MyStringTag>;
    using MyDouble = StrongType<double , MyDoubleTag>;

    MyInt x {10}, z{100};
    MyString y {"blah"};
    MyDouble w{1.4};

    auto insertStatement = makeInsertStatement(fieldId, fieldText, fieldCount, fieldValue);
    ASSERT_NO_THROW(insertStatement.attach(db, "Insert1"));
    ASSERT_NO_THROW(insertStatement.insert(x, y, z, w));

}

