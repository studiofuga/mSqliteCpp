
add_executable(runProfiler
        profiler.cpp
        ../include/msqlitecpp/v2/insertstatement.h ../include/msqlitecpp/v2/helpers/formatters.h)

if(UNIX AND NOT APPLE)
    set(EXTRA_LIB pthread)
endif(UNIX AND NOT APPLE)

# Extra linking for the project.
target_link_libraries(runProfiler msqlitecpp ${EXTRA_LIB})

#add_test(NAME profiler_st COMMAND runProfiler)
#add_test(NAME profiler_mt COMMAND runProfiler --mt)
