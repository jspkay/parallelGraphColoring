cmake_minimum_required(VERSION 3.17)
project(GraphColoring)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include_directories("./boost_1_77_0")
set(CMAKE_CXX_FLAGS -pthread)
add_definitions(-DMULTITHREAD_DEBUG)

add_executable(GraphColoring main.cpp
        Graph/Graph.h
        Graph/GraphCSR.cpp Graph/GraphAdjL.cpp Graph/GraphAdjM.cpp
        PrintMenu.h
        Graph/Algorithms.cpp)
