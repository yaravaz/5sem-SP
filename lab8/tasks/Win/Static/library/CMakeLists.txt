cmake_minimum_required(VERSION 3.10)
project(Mappings)

add_library(Mappings STATIC
    Mappings.c
    include/Mappings.h
)

target_include_directories(Mappings PUBLIC include)