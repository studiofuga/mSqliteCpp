
if (ENABLE_SQLITE_AMALGAMATION)
    add_definitions(-DEMBED_MSQLITECPP)
    add_definitions(-DBUILD_MSQLITECPP)

    find_file(SQLITE_SOURCE_PATH sqlite3.c
            HINT ${CMAKE_SOURCE_DIR}/sqlite-amalgamation)
    if(NOT SQLITE_SOURCE_PATH)
        message(FATAL_ERROR "Cannot find sqlite3.c for amalgamation")
    endif()

    # Up one component
    get_filename_component(SQLITE_SOURCE_PATH "${SQLITE_SOURCE_PATH}" PATH)
    set(SQLITE_INCLUDE_DIRS ${SQLITE_SOURCE_PATH})

else (ENABLE_SQLITE_AMALGAMATION)
    set (SQLITE_LIB sqlite3)
    add_definitions(-DBUILD_MSQLITECPP)
endif (ENABLE_SQLITE_AMALGAMATION)

if (ENABLE_CODECOVERAGE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif (ENABLE_CODECOVERAGE)

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif (APPLE)
