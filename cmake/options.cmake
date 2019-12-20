# Parse options

OPTION(BUILD_SHARED_LIBS "Build library as shared" ON)
OPTION(ENABLE_CONAN "Enable support for Conan package manager" Off)
OPTION(ENABLE_TEST "Enable Tests" ON)
OPTION(ENABLE_PROFILER "Enable Profiler" ON)
OPTION(ENABLE_CODECOVERAGE "Enable Code Coverage tests" OFF)
OPTION(DISABLE_LEGACY_CODE "Disable Legacy (v1) API" OFF)

if (ENABLE_CONAN)
    set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
endif(ENABLE_CONAN)

if (DISABLE_LEGACY_CODE)
    set(DISABLE_V1 on)
endif ()
