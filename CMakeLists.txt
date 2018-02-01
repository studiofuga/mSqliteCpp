cmake_minimum_required(VERSION 3.8)
project(sqlite)

OPTION(ENABLE_TEST "Enable Tests" ON)
OPTION(ENABLE_PROFILER "Enable Profiler" ON)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${SQLITE_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}
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
        src/sqlitestatement.cpp include/sqlitestatementformatters.h include/sqlitefieldsop.h)

add_definitions(-DBUILD_MSQLITECPP)
link_directories(${CMAKE_SOURCE_DIR})

add_library(msqlitecpp SHARED ${SOURCE_FILES})

target_link_libraries(msqlitecpp sqlite3)

if (ENABLE_TEST)
add_subdirectory(tests)
endif(ENABLE_TEST)

if (ENABLE_PROFILER)
    add_subdirectory(profiler)
endif(ENABLE_PROFILER)
