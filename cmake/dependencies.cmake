# Manage dependencies, find paths, etc

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
endif (ENABLE_SQLITE_AMALGAMATION)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.8.99)
    find_package(Doxygen)
endif (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.8.99)

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
endif (ENABLE_CODECOVERAGE)
