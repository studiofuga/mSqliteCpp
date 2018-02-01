##############
# Testing
##############

# Fix problems linking on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# This adds another subdirectory, which has 'project(gtest)'.
add_subdirectory(googletest)

#enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runTests
        table.cpp
        transactions.cpp
        issues.cpp
        statements.cpp sqlformatting.cpp)

target_link_libraries(runTests msqlitecpp gtest gtest_main)

add_test(NAME tests COMMAND runTests)
