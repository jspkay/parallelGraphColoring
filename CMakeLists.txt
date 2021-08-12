cmake_minimum_required(VERSION 3.20)
project(p2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(p2 main.cpp graph/graph.cpp graph/graph.h)
