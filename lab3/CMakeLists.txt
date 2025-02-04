cmake_minimum_required(VERSION 3.10)
project(Lab-03a-server C)

set(CMAKE_C_STANDARD 11)

add_executable(Lab-03b-server Lab-03b-server.c)
add_executable(Lab-03b-client Lab-03b-client.c)

find_package(Threads REQUIRED)

# Связываем нужные библиотеки с исполняемыми файлами
target_link_libraries(Lab-03b-server PRIVATE Threads::Threads)
target_link_libraries(Lab-03b-client PRIVATE Threads::Threads)
