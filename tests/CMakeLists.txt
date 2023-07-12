set(SOURCES
        version_test.cpp
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
            v1/TestForeignKeys.cpp v1/ToString.cpp)
endif ()

add_executable(runTests ${SOURCES})

target_include_directories(runTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${Boost_INCLUDE_DIRS}
        ${SQLite3_INCLUDE_DIRS}
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        )

target_compile_features(runTests PUBLIC cxx_std_14)

target_link_libraries(runTests
        msqlitecpp
        Boost::filesystem
        GTest::GTest GTest::Main
        ${SQLITE3_LIBRARY}
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

add_test(NAME runUnitTests
        COMMAND runTests)
