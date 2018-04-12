# mSqliteCpp {#mainpage}
_A modern SQLite C++ interface_

## Introduction
Why this library?

C++ is a strong typed language and the obvious advantage of this is the ability to catch many non-obvious problems at
compile time.

The SQLite C API library somehow try to enforce the strong typing of the database engine by using different functions
for different types, but all the C++ libraries I've seen till now don't make any effort to force strong types at user
level. They are just wrapper.

This library tries to enforce the strong typing by making wide usage of template metaprogramming.

Strong types are used in different context:

- When defining a table structure, either creating or querying it
- When running queries (insert, select), because there should be a strict relation between the fields selected and the
type returned by the query / passed to the insert

## Usage
Examples can be found in the `tests` directory. Below are a few examples:

### Creating a table

```
    auto db = std::make_shared<SQLiteStorage>(":memory:");
    db->open();
    auto fldId = makeFieldDef("id", FieldType::Integer()).primaryKey().autoincrement();
    auto fldName = makeFieldDef("name", FieldType::Text());
    auto fldCount = makeFieldDef("count", FieldType::Integer());
    auto tableDefinition = std::make_tuple(
            fldId,
            fldName,
            fldCount
    );

    auto table = SQLiteTable::create(db, "tableName", tableDefinition));
```

### Querying a table

```
    std::vector<Record> r;
    SQLiteTable::query(std::make_tuple(fldName, fldCount), [&r](std::string name, int value) {
        r.push_back(Record{name, value});
    });
```

### Querying a table, filtering with WHERE clause

```
    auto tb = std::make_tuple(fldName, fldCount);
    auto where = std::make_tuple(fldCount.assign(100));
    int cnt = 0;
    table.query(tb, where, [&r](std::string name, int value) { ++cnt; }));
```

## Building
At the time of writing, the library depends only from the SQLite C Api library (libsqlite3).

Before compiling, you must clone the googletest directory in `tests/googletest`, or disable testing adding
`-DENABLE_TEST=OFF` while running cmake.

To clone the googletest, simply run the following command from the command line:

```
$ git clone https://github.com/google/googletest.git tests/googletest
```

## License
The library is released under the BSD 2-Clause license. See the LICENSE file for details.
