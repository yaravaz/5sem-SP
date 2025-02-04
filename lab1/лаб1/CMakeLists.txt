cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_C_STANDARD 11) 

add_executable(MyExecutable
    main.c
    Array.c
    Pointer.c
    Array_tests.c
    Pointer_tests.c
)