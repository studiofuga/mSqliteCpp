##############
# Testing
##############


add_executable(runTestsV1
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

target_include_directories(runTestsV1
        PRIVATE
            ${Boost_INCLUDE_DIRS}
            ${SQLITE_INCLUDE_DIRS}
            ${gtest_SOURCE_DIR}/include
            ${gtest_SOURCE_DIR}
)

target_compile_features(runTestsV1 PUBLIC cxx_std_14)

target_link_libraries(runTestsV1
        msqlitecpp
        gtest gtest_main
        ${SQLITE_LIB}
        )

add_custom_target(checkV1
        COMMAND runTestsV1 --gtest_output=xml:gtestresults.xml
        DEPENDS runTestsV1)

if (ENABLE_CODECOVERAGE)
    add_custom_target(codecoverage
            DEPENDS runTestsV1
            COMMAND runTestsV1 --gtest_output=xml:gtestresults.xml || true
            COMMAND lcov -t "CodeCoverage Tests" -o unittests.info -c -d ${CMAKE_BINARY_DIR}
            COMMAND lcov --remove unittests.info '/usr/include/*' '*/googletest/*' > unittests_rm.info
            COMMAND mv unittests_rm.info unittests.info
            COMMAND genhtml -o codecoverage unittests.info
            )
endif (ENABLE_CODECOVERAGE)

add_test(UnitTests runTestsV1)