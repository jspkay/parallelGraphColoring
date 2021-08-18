cmake_minimum_required(VERSION 3.20)
project(p2)

include_directories("C:/Program Files/Boost/boost_1_77_0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG )

add_executable(p2 main.cpp graph/graph.cpp graph/graph.h)
