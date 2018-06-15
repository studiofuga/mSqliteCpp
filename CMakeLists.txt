cmake_minimum_required(VERSION 3.5)
project(sqlite)

# Check if this file is the main project file or not (used as a subproject)
if (NOT ${CMAKE_CURRENT_LIST_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(MSQLITECPP_ROOT ${CMAKE_CURRENT_LIST_DIR})
    set(MSQLITECPP_SUBPROJECT true)
else (NOT ${CMAKE_CURRENT_LIST_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(MSQLITECPP_ROOT ${CMAKE_SOURCE_DIR})
endif (NOT ${CMAKE_CURRENT_LIST_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

OPTION(ENABLE_TEST "Enable Tests" ON)
OPTION(ENABLE_PROFILER "Enable Profiler" ON)
OPTION(ENABLE_SQLITE_AMALGAMATION "Compile SQLite Amalgamation" OFF)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${MSQLITECPP_ROOT})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost)

if (Boost_FOUND)
    add_definitions(-DWITH_BOOST)
endif(Boost_FOUND)

if (${CMAKE_VERSION} VERSION_GREATER 3.8.99)
    find_package(Doxygen)
endif (${CMAKE_VERSION} VERSION_GREATER 3.8.99)

include_directories(
        ${MSQLITECPP_ROOT}/include
        ${SQLITE_INCLUDE_DIRS}
        ${MSQLITECPP_ROOT}
)

set(SOURCE_FILES
        include/msqlitecpp.h
        include/sqlitefielddef.h
        src/sqlitestorage.cpp
        include/sqlitestorage.h
        src/sqlitetable.cpp
        include/sqlitetable.h
        src/sqlitetransaction.cpp
        include/sqlitetransaction.h
        include/sqlitestatement.h
        src/sqlitestatement.cpp include/sqlitestatementformatters.h include/sqlitefieldsop.h
        src/utils/make_unique.h include/insertstatement.h include/selectstatement.h include/clauses.h include/deletestatement.h include/updatestatement.h include/createstatement.h include/sqlformatters_helpers.h)

if (ENABLE_SQLITE_AMALGAMATION)
    set (SOURCE_FILES ${SOURCE_FILES}
            ${SQLITE_INCLUDE_DIRS}/sqlite3.c
        )
    set (LINK_SHARED STATIC)
    add_definitions(-DEMBED_MSQLITECPP)
else (ENABLE_SQLITE_AMALGAMATION)
    set (SQLITE_LIB sqlite3)
    add_definitions(-DBUILD_MSQLITECPP)
    set (LINK_SHARED SHARED)
endif (ENABLE_SQLITE_AMALGAMATION)

link_directories(${MSQLITECPP_ROOT})

add_library(msqlitecpp ${LINK_SHARED} ${SOURCE_FILES})

target_link_libraries(msqlitecpp ${SQLITE_LIB})

if (ENABLE_TEST)
    add_subdirectory(tests)
endif(ENABLE_TEST)

if (ENABLE_PROFILER)
    add_subdirectory(profiler)
endif(ENABLE_PROFILER)

if (DOXYGEN_FOUND)
    list(APPEND DOXYGEN_EXCLUDE_PATTERNS
            */tests/googletest
            )
    doxygen_add_docs(documentation)
endif (DOXYGEN_FOUND)
