cmake_minimum_required(VERSION 3.17)
project(p2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(p2 main.cpp asa/graph/Graph.cpp asa/graph/Graph.h asa/graph/benchmark/rgg_n_2_15_s0.txt asa/graph/graph_csr.cpp asa/graph/graph_csr.h asa/graphColoring.cpp asa/graphColoring.h asa/graph/Graph_simple.cpp asa/graph/Graph_simple.h)
include_directories("./boost_1_77_0")
