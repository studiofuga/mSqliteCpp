##############
# Testing
##############


add_executable(runTestsV2
        storage.cpp
        statements.cpp
        insertstatements.cpp fields.cpp)

target_include_directories(runTestsV2
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${SQLITE_INCLUDE_DIRS}
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        )

target_compile_features(runTestsV2 PUBLIC cxx_std_14)

target_link_libraries(runTestsV2
        msqlitecpp2
        gtest gtest_main
        ${SQLITE_LIB}
        )

add_custom_target(check
        COMMAND runTestsV2 --gtest_output=xml:gtestresults.xml
        DEPENDS runTestsV2)

if (ENABLE_CODECOVERAGE)
    add_custom_target(codecoverage
            DEPENDS runTestsV2
            COMMAND runTestsV2 --gtest_output=xml:gtestresults.xml || true
            COMMAND lcov -t "CodeCoverage Tests" -o unittests.info -c -d ${CMAKE_BINARY_DIR}
            COMMAND lcov --remove unittests.info '/usr/include/*' '*/googletest/*' > unittests_rm.info
            COMMAND mv unittests_rm.info unittests.info
            COMMAND genhtml -o codecoverage unittests.info
            )
endif (ENABLE_CODECOVERAGE)

add_test(UnitTests runTestsV2)