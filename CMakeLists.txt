cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
project(life-sim)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_executable(life-sim
        main.cpp
        shader.cpp
        sprite_renderer.cpp
        texture.cpp
        resource_manager.cpp
        glad.c
        text_renderer.h
        text_renderer.cpp
        font.h
        game.h
        game.cpp
        NeuralNetwork.cpp
        NeuralNetwork.h
        board.h
        board.cpp
        BacteriaData.h
        simulation_engine.cpp
        simulation_engine.h
        BacteriaData.cpp

)
include_directories(life-sim PRIVATE include)
target_link_libraries(life-sim
        PRIVATE
        glfw
        glad::glad
        glm::glm
        freetype::freetype
)