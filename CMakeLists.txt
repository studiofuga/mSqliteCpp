cmake_minimum_required(VERSION 3.8)
project(sqlite)

OPTION(ENABLE_TEST "Enable Tests" ON)

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
        src/sqlitetransaction.cpp
        include/sqlitetransaction.h
        include/sqlitestatement.h
        src/sqlitestatement.cpp)

add_definitions(-DBUILD_MSQLITECPP)
link_directories(${SQLITE_LIBS_PATH})

add_library(msqlitecpp SHARED ${SOURCE_FILES})

target_link_libraries(msqlitecpp sqlite3)

if (ENABLE_TEST)
add_subdirectory(tests)
endif(ENABLE_TEST)
