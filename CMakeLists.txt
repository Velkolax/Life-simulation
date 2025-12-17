cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
project(life-sim)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(freetype)
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
target_link_libraries(life-sim ${GLFW3_LIBRARY} OpenGL::GL freetype)

