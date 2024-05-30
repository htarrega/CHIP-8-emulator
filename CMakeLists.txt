project(chip8emu)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(emu src/emu.cpp src/components/components.cpp src/instructions/instructions.cpp)
#add_executable(loader src/8oLoader.cpp)