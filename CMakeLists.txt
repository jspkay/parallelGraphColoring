cmake_minimum_required(VERSION 3.17)
project(p2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(p2 main.cpp Graph/Graph.h Graph/benchmark/rgg_n_2_15_s0.txt Graph/GraphCSR.cpp Graph/GraphAdjL.cpp Graph/GraphAdjM.cpp)
include_directories("/home/antonio_vespa/Documenti/boost_1_77_0")
set(CMAKE_CXX_FLAGS -pthread)