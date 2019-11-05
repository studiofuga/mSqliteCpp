project(TestMsqliteCppCmake)
cmake_minimum_required(VERSION 3.10)

find_package(msqlitecpp 1.99.3 REQUIRED)

add_executable(ex_insert main.cpp)

target_link_libraries(ex_insert msqlitecpp::msqlitecpp)