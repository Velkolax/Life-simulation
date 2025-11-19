cmake_minimum_required(VERSION 4.0)
project(Life_simulation)

set(CMAKE_CXX_STANDARD 20)
include_directories(Life-simulation)

add_executable(Life_simulation
        NeuralNetwork.h
        NeuralNetwork.c
        Bacteria.cpp
        Bacteria.h
        LifeSimulator.cpp
        LifeSimulator.h
)
target_link_libraries(Life_simulation m)

