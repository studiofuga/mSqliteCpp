cmake_minimum_required(VERSION 3.8)
project(sqlite)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${SQLITE_INCLUDE_DIRS}
)

set(SOURCE_FILES
        include/sqlitefielddef.h
        src/sqlitestorage.cpp
        include/sqlitestorage.h
        src/sqlitetable.cpp
        include/sqlitetable.h
        )


add_library(msqlitecpp SHARED ${SOURCE_FILES})

target_link_libraries(msqlitecpp sqlite3)
