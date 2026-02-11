cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
project(life-sim)

if(WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

set(CMAKE_PREFIX_PATH  "/opt/Qt/6.10.2/gcc_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets OpenGL)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_executable(life-sim
        main.cpp
        shader.cpp
        sprite_renderer.cpp
        texture.cpp
        resource_manager.cpp
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
        game_configdata.cpp
        game_configdata.h

)
include_directories(life-sim PRIVATE include)
target_link_libraries(life-sim
        PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        Qt6::OpenGL
        glfw
        glm::glm
        freetype
)

if(MINGW)
    target_link_options(life-sim PRIVATE -static-libgcc -static-libstdc++ -static)
endif()