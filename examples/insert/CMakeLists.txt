find_package(msqlitecpp REQUIRED)

add_executable(ex_insert main.cpp)

target_link_libraries(ex_insert msqlitecpp::msqlitecpp)