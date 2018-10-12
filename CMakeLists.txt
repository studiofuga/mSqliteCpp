cmake_minimum_required(VERSION 3.5)
project(msqlitecpp VERSION 0.9.1 DESCRIPTION "a Modern SQLite CPP library" LANGUAGES C CXX)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/compileropts.cmake)
include(cmake/install.cmake)

include (main.cmake)

if (ENABLE_TEST)
    enable_testing()

    add_subdirectory(tests)
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
