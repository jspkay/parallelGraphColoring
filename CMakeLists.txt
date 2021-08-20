cmake_minimum_required(VERSION 3.17)
project(p2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(p2 main.cpp graph/Graph.cpp graph/Graph.h graph/benchmark/rgg_n_2_15_s0.txt)
