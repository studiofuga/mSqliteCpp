cmake_minimum_required(VERSION 3.8)
project(msqlitecpp VERSION 1.0.1 LANGUAGES C CXX)

set(msqlitecpp_DESCRIPTION "a Modern SQLite CPP library")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/compileropts.cmake)
include(cmake/install.cmake)

add_subdirectory(src/v1)
add_subdirectory(src/v2)

if (ENABLE_TEST)
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
    add_subdirectory(tests/googletest EXCLUDE_FROM_ALL)

    enable_testing()

    add_subdirectory(tests/v1)
    add_subdirectory(tests/v2)
endif(ENABLE_TEST)

if (ENABLE_PROFILER)
    add_subdirectory(profiler)
endif(ENABLE_PROFILER)

if (DOXYGEN_FOUND)
    list(APPEND DOXYGEN_EXCLUDE_PATTERNS
            */tests/googletest
            )
    doxygen_add_docs(documentation)
endif (DOXYGEN_FOUND)
