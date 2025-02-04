cmake_minimum_required(VERSION 3.10)

project(Lab-08b)

set(SOURCE_FILES src/dllmain.c src/Lab-08b.c)

include_directories(include)

add_library(Mappingd SHARED src/dllmain.c src/Mappingd.def)

add_executable(Lab-08b src/Lab-08b.c)

target_link_libraries(Lab-08b Mappingd)