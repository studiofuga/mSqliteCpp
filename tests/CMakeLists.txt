##############
# Testing
##############

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
add_subdirectory(googletest)

#enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runTests
        table.cpp
        transactions.cpp
        issues.cpp
        statements.cpp sqlformatting.cpp clauses.cpp updatestatements.cpp)

target_link_libraries(runTests msqlitecpp gtest gtest_main)

add_test(NAME tests COMMAND runTests)
