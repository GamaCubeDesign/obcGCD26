cmake_minimum_required(VERSION 3.13)
project(obc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

option(OBC_SIM "Injeta telecomandos pelo teclado em vez do radio" OFF)

if(OBC_SIM)
    set(RADIO_SRC src/drivers/radio_sim.c)
else()
    set(RADIO_SRC src/drivers/radio.c)
endif()

add_executable(obc
    src/main.c
    src/fsm.c
    src/obclog.c
    ${RADIO_SRC}
)

target_include_directories(obc PRIVATE src)
