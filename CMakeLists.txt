cmake_minimum_required(VERSION 3.20)

project(Chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(Chip8 ${SOURCES})

target_include_directories(Chip8 PRIVATE include)
find_package(SDL2 REQUIRED)

target_link_libraries(Chip8 PRIVATE
    SDL2::SDL2
)