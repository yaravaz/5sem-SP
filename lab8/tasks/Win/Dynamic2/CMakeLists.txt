cmake_minimum_required(VERSION 3.10)

project(Lab-08b)

set(SOURCE_FILES src/dllmain.c src/Lab-08b.c)

include_directories(include)

add_library(Mappingd-2 SHARED src/dllmain.c)

add_executable(Lab-08b src/Lab-08b.c)

target_link_libraries(Lab-08b Mappingd-2)