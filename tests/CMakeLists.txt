##############
# Testing
##############

# This adds another subdirectory, which has 'project(gtest)'.
add_subdirectory(googletest)

enable_testing()

# Include the gtest library. gtest_SOURCE_DIR is available due to
# 'project(gtest)' above.
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runTests
        table.cpp
        transactions.cpp
        )

# Standard linking to gtest stuff.
target_link_libraries(runTests gtest gtest_main)

# Extra linking for the project.
target_link_libraries(runTests msqlitecpp)

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable runUnitTests to see those specific tests.
add_test(NAME tests COMMAND runTests)
