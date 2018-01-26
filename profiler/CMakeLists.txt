
add_executable(runProfiler
        profiler.cpp
    )

# Extra linking for the project.
target_link_libraries(runProfiler msqlitecpp)

add_test(NAME profiler_st COMMAND runProfiler)
add_test(NAME profiler_mt COMMAND runProfiler --mt)
