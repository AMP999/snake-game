cmake_minimum_required(VERSION 3.20)
project(snake-game VERSION 0.1.0)

find_library(raylib raylib)
add_executable(snake-game main.cpp)
target_link_libraries(snake-game raylib)