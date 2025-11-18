cmake_minimum_required(VERSION 4.0)
project(Life_simulation)

set(CMAKE_CXX_STANDARD 20)
include_directories(Life-simulation)

add_executable(Life_simulation
        sieci.h
        Bacteria.cpp
        Bacteria.h)
target_link_libraries(Life_simulation m)

