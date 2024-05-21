project(chip8emu)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

add_executable(emu emu.cpp)