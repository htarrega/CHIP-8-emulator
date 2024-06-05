project(chip8emu)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Define build types
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0") 
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG") 

add_executable(emu src/emu.cpp src/components/components.cpp src/instructions/instructions.cpp)