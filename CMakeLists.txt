cmake_minimum_required(VERSION 4.0)
project(Life_simulation C)

set(CMAKE_C_STANDARD 11)
include_directories(Life-simulation)

add_executable(Life_simulation
        sieci.c)
target_link_libraries(Life_simulation m)

