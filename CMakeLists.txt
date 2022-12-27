cmake_minimum_required(VERSION 3.23)
project(Chippy)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Chippy src/main.cpp src/chip8.cpp src/chip8.h src/Display.cpp src/Display.h src/stack.cpp src/stack.h)
target_link_libraries(Chippy ${SDL2_LIBRARIES})
