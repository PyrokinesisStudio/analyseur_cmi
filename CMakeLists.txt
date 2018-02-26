cmake_minimum_required (VERSION 2.6)
project(Main)

add_subdirectory(source/)
add_subdirectory(source/analyzer/)

include_directories("lib/")
include_directories("source/")
include_directories("source/analyzer/")

add_executable(Main source/main.cpp)
target_link_libraries(Main Common)
target_link_libraries(Main Analyzer)
