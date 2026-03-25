cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
project(life-sim)

if(WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH "/opt/Qt/6.10.2/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets OpenGL)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_library(sim-logic STATIC
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
        main_window.cpp
        main_window.h

)
target_include_directories(sim-logic PUBLIC include)
target_link_libraries(sim-logic
        PUBLIC
        Qt6::Widgets
        Qt6::OpenGLWidgets
        Qt6::OpenGL
        glfw
        glm::glm
        freetype
)

add_executable(life-sim main.cpp)
target_link_libraries(life-sim PRIVATE sim-logic)

enable_testing()
add_executable(unit_tests
    tests/test_main.cpp
)

target_link_libraries(unit_tests
    PRIVATE
        sim-logic
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(unit_tests)

if(MINGW)
    target_link_options(life-sim PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(unit_tests PRIVATE -static-libgcc -static-libstdc++ -static)
endif()
