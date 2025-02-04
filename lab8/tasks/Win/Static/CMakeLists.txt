cmake_minimum_required(VERSION 3.10)
project(Lab-08a)

add_subdirectory(library)

add_executable(Lab-08a Lab-08a.c)

target_link_libraries(Lab-08a PRIVATE Mappings)