# Fix problems linking on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/tests/googletest/CMakeLists.txt)
    set(GTEST_URL https://github.com/google/googletest.git)
    execute_process(
            COMMAND git clone ${GTEST_URL}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif (NOT EXISTS ${CMAKE_SOURCE_DIR}/tests/googletest/CMakeLists.txt)

# This adds another subdirectory, which has 'project(gtest)'.
add_subdirectory(googletest EXCLUDE_FROM_ALL)

enable_testing()

##############
# Testing
##############

set(SOURCES
        v2/storage.cpp
        v2/statements.cpp
        v2/insertstatements.cpp
        v2/fields.cpp
        v2/createstatement.cpp
        v2/selectstatements.cpp
        )


if (NOT DISABLE_V1)
    set(SOURCES ${SOURCES}
            v1/table.cpp
            v1/transactions.cpp
            v1/issues.cpp
            v1/statements.cpp
            v1/sqlformatting.cpp
            v1/clauses.cpp
            v1/updatestatements.cpp
            v1/OptionalTests.cpp
            v1/NullsTest.cpp
            v1/StrongTypeTests.cpp
            v1/WhereOptTests.cpp
            v1/TestForeignKeys.cpp)
endif ()

add_executable(runTests ${SOURCES})

target_include_directories(runTests
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${SQLITE_INCLUDE_DIRS}
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        )

target_compile_features(runTests PUBLIC cxx_std_14)

target_link_libraries(runTests
        msqlitecpp
        Boost::filesystem
        gtest gtest_main
        ${SQLITE_LIB}
        )

add_custom_target(checkV1
        COMMAND runTests --gtest_output=xml:gtestresults.xml
        DEPENDS runTests)

if (ENABLE_CODECOVERAGE)
    add_custom_target(codecoverage
            DEPENDS runTests
            COMMAND runTests --gtest_output=xml:gtestresults.xml || true
            COMMAND lcov -t "CodeCoverage Tests" -o unittests.info -c -d ${CMAKE_BINARY_DIR}
            COMMAND lcov --remove unittests.info '/usr/include/*' '*/googletest/*' > unittests_rm.info
            COMMAND mv unittests_rm.info unittests.info
            COMMAND genhtml -o codecoverage unittests.info
            )
endif (ENABLE_CODECOVERAGE)

add_test(UnitTests runTests)