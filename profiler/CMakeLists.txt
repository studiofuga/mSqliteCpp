
if (NOT DISABLE_V1)
    add_executable(runProfiler profiler.cpp)

    if (UNIX AND NOT APPLE)
        set(EXTRA_LIB pthread)
    endif (UNIX AND NOT APPLE)

	target_include_directories(runProfiler
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${SQLite3_INCLUDE_DIRS}
        )
		
	target_compile_features(runProfiler PUBLIC cxx_std_14)

    # Extra linking for the project.
    target_link_libraries(runProfiler
            msqlitecpp
            ${SQLITE3_LIBRARY}
            ${EXTRA_LIB}
            )

endif ()
