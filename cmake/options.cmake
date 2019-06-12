# Parse options

OPTION(BUILD_SHARED_LIBS "Build library as shared" ON)
OPTION(ENABLE_TEST "Enable Tests" ON)
OPTION(ENABLE_PROFILER "Enable Profiler" ON)
OPTION(ENABLE_CODECOVERAGE "Enable Code Coverage tests" OFF)
OPTION(DISABLE_LEGACY_CODE "Disable Legacy (v1) API" OFF)

if (DISABLE_LEGACY_CODE)
    set(DISABLE_V1 on CACHE)
endif ()
