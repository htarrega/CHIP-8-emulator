project(chip8emu)
cmake_minimum_required(VERSION 3.0)

# Set compilers
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Set flags for Debug and Release builds
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)

# Print the final flags (for debugging CMake itself)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Add the executable
add_executable(emu src/emu.cpp src/components/components.cpp src/instructions/instructions.cpp src/cpu/cpu.cpp)

# Link SDL2 libraries
target_link_libraries(emu ${SDL2_LIBRARIES})
