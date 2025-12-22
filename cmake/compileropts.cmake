
if (ENABLE_CODECOVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif (ENABLE_CODECOVERAGE)

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif (APPLE)

set(CMAKE_POSITION_INDEPENDENT_CODE on)

if ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    set(ASAN_ENABLED ON)
    message("Address Sanitizer: ENABLED")
endif ()

if (ASAN_ENABLED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined,leak -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined,leak -fno-omit-frame-pointer")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined,leak")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address,undefined,leak")

    add_compile_definitions(-DASAN_ENABLED)
endif()

# Always ensure debug symbols and frame pointers in Debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -ggdb")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -ggdb")
