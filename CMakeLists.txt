cmake_minimum_required(VERSION 3.8)
project(sqlite)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include_directories(${SQLITE_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp
        sqlitefielddef.h
        sqlitestorage.cpp
        sqlitestorage.h
        sqlitetable.cpp
        sqlitetable.h
        )


add_executable(sqlitetest ${SOURCE_FILES})

target_link_libraries(sqlitetest sqlite3)
