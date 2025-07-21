cmake_minimum_required(VERSION 3.14)

# Project Name and Version
project(OpEng VERSION 0.1 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES
     ${PROJECT_SOURCE_DIR}/src/*.cpp
)
include_directories(${CMAKE_SOURCE_DIR}/include/eigen)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

