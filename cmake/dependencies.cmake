# Manage dependencies, find paths, etc

find_package(Boost 1.71.0 REQUIRED COMPONENTS filesystem)

if (Boost_FOUND)
    add_definitions(-DWITH_BOOST)
endif(Boost_FOUND)

find_package(SQLite3 3.35.0 REQUIRED)

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

if (ENABLE_TEST)
    include(cmake/googletest.cmake)
#    message("Try: ${CMAKE_SOURCE_DIR}/googletest")
#    find_package(GTest REQUIRED HINTSw ${CMAKE_SOURCE_DIR}/googletest/googletest)
endif (ENABLE_TEST)
