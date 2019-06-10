##############
# Testing
##############


add_executable(runTests
        table.cpp
        transactions.cpp
        issues.cpp
        statements.cpp
        sqlformatting.cpp
        clauses.cpp
        updatestatements.cpp
        OptionalTests.cpp
        NullsTest.cpp
        StrongTypeTests.cpp
        WhereOptTests.cpp
        TestForeignKeys.cpp)

target_include_directories( runTests
        PRIVATE
            ${Boost_INCLUDE_DIRS}
            ${SQLITE_INCLUDE_DIRS}
            ${gtest_SOURCE_DIR}/include
            ${gtest_SOURCE_DIR}
)

target_compile_features(runTests PUBLIC cxx_std_14)

target_link_libraries(runTests
        msqlitecpp
        gtest gtest_main
        ${SQLITE_LIB}
        )

add_custom_target(check
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