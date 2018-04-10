# mSqliteCpp API description

## Introduction

This document describes the API of mSqliteCpp, subdivided by topics.

## The SQLiteStorage Object

A Database is represented by an instance of the SQLiteStorage class.

The SQLiteStorage Object is the most important object of the API. It keeps and manages the handles to the
sqlite library, and most of the other objects need to have a reference to an open Database object.

All the API is designed to use reference through smart pointers, in particular std::shared_ptr.

### Database Opening

A database is opened using the SQLiteStorage class.
Once the proper object is created, the open() function is used to open it (on the disk or in memory).
Then the object can be used with the other API classes, passing a proper std::shared_ptr.

```
auto db = std::make_shared<SQLiteStorage>(":memory:");
try {
    db->open();
} catch (sqlite::SQLiteException &x) {
}
```

## Tables

The SQLiteTable class manages a table representation of a Database table. It is not required to manage a
table on a db, because they can be accessed in various ways. But a Table is a fixed structure of the db, while
a statement can be used on a table in different ways. For example, you may decide to manage a table using all the
fields of the schema, but you can also decide to use a subset of it.

To allow such a flexibility, mSqliteCpp allows you to manage a table using a SQLiteTable class or other objects
representing statements. It's up to you to decide.

