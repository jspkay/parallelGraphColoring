cmake_minimum_required(VERSION 3.17)
project(p2)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-10)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -std=c++2a -fconcepts")

add_executable(p2 main.cpp
        asa/Graph.h #asa/Graph.cpp
        asa/graph/graph_csr.cpp asa/graph/graph_csr.h
        asa/graphColoring.cpp asa/graphColoring.h
        asa/graph/Graph_simple.cpp asa/graph/Graph_simple.h
        asa/Input.h)

include_directories("./boost_1_77_0")
