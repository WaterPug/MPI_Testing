cmake_minimum_required(VERSION 3.14)

project(MPI_Testing VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(MPI REQUIRED)

add_subdirectory(Example1)
add_subdirectory(GameOfLife)