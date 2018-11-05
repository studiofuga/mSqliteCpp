# Parse options

OPTION(BUILD_SHARED_LIBS "Build library as shared" ON)
OPTION(WITH_CONAN "Use Conan.io to manage dependencies" OFF)
OPTION(ENABLE_TEST "Enable Tests" ON)
OPTION(ENABLE_PROFILER "Enable Profiler" ON)
OPTION(ENABLE_SQLITE_AMALGAMATION "Compile SQLite Amalgamation" OFF)
OPTION(ENABLE_CODECOVERAGE "Enable Code Coverage tests" OFF)

if (WITH_CONAN)
    if (NOT WITH_CONAN_DIR)
        set (WITH_CONAN_DIR ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    include(${WITH_CONAN_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()
