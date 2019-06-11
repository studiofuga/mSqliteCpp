# mSqliteCpp {#mainpage}
_A modern SQLite C++ interface_

## Introduction
Why this library?

C++ is a strong typed language and the very important advantage of this is the ability to catch many non-obvious 
problems at compile time.

The SQLite C API library somehow try to enforce the strong typing of the database engine by using different functions
for different types, but all the C++ libraries I've seen till now don't make any effort to force strong types at user
level. They are just wrapper.

This library tries to enforce the strong typing by making wide usage of template metaprogramming.

Strong types are used in different context:

- When defining a table structure, either creating or querying it
- When running queries (insert, select), because there should be a strict relation between the fields selected and the
type returned by the query / passed to the insert

## V1 vs V2

There are two versions of the API.

V1 is my first attempt to implement the intended behaviour. It is a complete but pretty messed set of classes, 
where I was trying to understand how was the best way to achieve my objective of creating a useful and 
flexible library of classes. It uses metaprogramming and classic C++ approach.

It implements CREATE TABLE/INDEX, INSERT INTO, SELECT, DELETE and UPDATE sql clauses but the use is sometimes difficult
and there are may different approach for the same function. Though it is used in some production project, I wouldn't 
suggest to use it.

V2 is the NEW API. It is a pretty new approach to the problem, I'm using metaprogramming and still classic c++ to 
encapsulate the sqlite functions. I'm keeping the good parts of the old API, removing the redundant and verbose
functions, and it will be official API. This documentation will cover only the V2 Api. V1 will be removed and 
abandoned one day.

Both apis can be installed side by side. For the moment, two libraries will be built, perhaps I'll unify them one day,
or perhaps no, we'll see. Headers are well separed in proper subdirectories. You can use both safely. 

## Usage
Examples can be found in the `tests` directory. Below are a few examples:

### Creating a table

```
    char const *SelectStatements::tableName = "t";

    Column<ColumnTypes::Integer> int1{"field1"};
    Column<ColumnTypes::Integer> int2{"field2"};
    Column<ColumnTypes::Text> text{"textfield"};

    auto db = Storage::inMemory();
    makeCreateStatement(db, tableName, int1, int2, text).execute();
```

### Inserting values

```
    auto insertStatement = makeInsertStatement(db, tableName, int1, int2, text);
    
    insertStatement.insert(1, 1, "1-1");
    insertStatement.insert(1, 2, "1-2");
    insertStatement.insert(2, 1, "2-1");    
```

### Executing queries with Select

```
    auto statement = makeSelectStatement(db, tableName, int1, int2, text);
    size_t count = 0;
    statement.execute([&count](int i1, int i2, std::string t) {
        ++count;
        return true;
    });
```

### Querying a table, filtering with WHERE clause

```
    auto whereClause = (int1 == "x" && int2 == "y");

    select.where(whereClause);
    select.bind(1,1);
    statement.execute([&count](int i1, int i2, std::string t) { 
        //...
    });
```

## Building
At the time of writing, the library depends only from the SQLite C Api library (libsqlite3).

To build the project, you need CMake.

The main project provides some compile time options. 

- ENABLE_TEST: enable compiling the unit tests suite, using Google Tests. Default is On

- ENABLE_PROFILER: enable compiling the profiler test program to measure the performances of the library.
The default is On

- ENABLE_CODECOVERAGE: compile the library with gcov support. Default is Off

## License
The library is released under the BSD 3-Clause license. See the LICENSE file for details.
