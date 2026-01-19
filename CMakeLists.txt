cmake_minimum_required(VERSION 3.22)
project(assert-cegar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all cpp files automatically
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable
add_executable(assertcegar main.cpp ${SOURCES})