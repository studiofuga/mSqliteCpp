cmake_minimum_required(VERSION 3.8)
project(msqlitecpp VERSION 1.99.7.1 LANGUAGES C CXX)

set(msqlitecpp_DESCRIPTION "a Modern SQLite CPP library")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/compileropts.cmake)
include(cmake/install.cmake)

if (ENABLE_TEST)
    enable_testing()
endif(ENABLE_TEST)

add_subdirectory(src)

if (ENABLE_TEST)
    add_subdirectory(tests)
endif (ENABLE_TEST)

if (ENABLE_PROFILER)
    add_subdirectory(profiler)
endif (ENABLE_PROFILER)

if (DOXYGEN_FOUND)
    list(APPEND DOXYGEN_EXCLUDE_PATTERNS
            */tests/googletest
            )
    doxygen_add_docs(documentation)
endif (DOXYGEN_FOUND)
