cmake_minimum_required(VERSION 3.20)
project(p2)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-D_GLIBCXX_DEBUG)


add_executable(p2 main.cpp graph/graph.cpp graph/graph.h )#coloring/coloring.cpp coloring/coloring.h)

