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
OPTION(ENABLE_CODECOVERAGE "Enable Code Coverage tests" OFF)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${MSQLITECPP_ROOT})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost)

if (Boost_FOUND)
    add_definitions(-DWITH_BOOST)
endif(Boost_FOUND)

if (ENABLE_SQLITE_AMALGAMATION)
    find_path(SQLITE_INCLUDE_PATH
            NAME sqlite3.h
            PATHS ${SQLITE_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/${SQLITE_INCLUDE_DIRS}
            )
    find_path(SQLITE_SOURCE_PATH
            NAME sqlite3.c
            PATHS ${SQLITE_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/${SQLITE_INCLUDE_DIRS}
            )
    set (LINK_SHARED STATIC)
    add_definitions(-DEMBED_MSQLITECPP)
    add_definitions(-DBUILD_MSQLITECPP)
else (ENABLE_SQLITE_AMALGAMATION)
    set (SQLITE_LIB sqlite3)
    add_definitions(-DBUILD_MSQLITECPP)
    set (LINK_SHARED SHARED)
endif (ENABLE_SQLITE_AMALGAMATION)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.8.99)
    find_package(Doxygen)
endif (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.8.99)

set(SOURCE_FILES
        include/msqlitecpp.h
        include/sqlitefielddef.h
        include/sqlitestorage.h
        include/sqlitetable.h
        include/sqlitetransaction.h
        include/sqlitestatement.h
        include/insertstatement.h
        include/selectstatement.h
        include/clauses.h
        include/deletestatement.h
        include/updatestatement.h
        include/createstatement.h
        include/sqlformatters_helpers.h
        include/sqlitestatementformatters.h
        include/sqlitefieldsop.h
        src/sqlitestorage.cpp
        src/sqlitetable.cpp
        src/sqlitetransaction.cpp
        src/sqlitestatement.cpp
        src/utils/make_unique.h
        )

include_directories(
        ${MSQLITECPP_ROOT}/include
        ${SQLITE_INCLUDE_DIRS}
        ${MSQLITECPP_ROOT}
)

if (ENABLE_SQLITE_AMALGAMATION)
    list(APPEND SOURCE_FILES ${SQLITE_SOURCE_PATH}/sqlite3.c)
    include_directories(${SQLITE_INCLUDE_PATH})
endif (ENABLE_SQLITE_AMALGAMATION)

if (ENABLE_CODECOVERAGE)
    message ("Code Coverage enabled")

    find_program( GCOV_PATH gcov )
    find_program( LCOV_PATH lcov )
    find_program( GENHTML_PATH genhtml )

    if(NOT GCOV_PATH)
        message(FATAL_ERROR "gcov not found" )
    endif(NOT GCOV_PATH)
    if(NOT LCOV_PATH)
        message(FATAL_ERROR "lcov not found" )
    endif(NOT LCOV_PATH)
    if(NOT GENHTML_PATH)
        message(FATAL_ERROR "genhtml not found" )
    endif(NOT GENHTML_PATH)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif (ENABLE_CODECOVERAGE)

link_directories(${MSQLITECPP_ROOT})

add_library(msqlitecpp ${LINK_SHARED} ${SOURCE_FILES})

target_link_libraries(msqlitecpp ${SQLITE_LIB})

install(TARGETS msqlitecpp DESTINATION usr/lib COMPONENT runtime)

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
