cmake_minimum_required(VERSION 3.13)
project(obc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(obc
    src/main.c
    src/fsm.c
    src/drivers/radio.c
)

target_include_directories(obc PRIVATE src)
