cmake_minimum_required(VERSION 3.17)
project(GraphColoring)

set(CMAKE_CXX_STANDARD 17)

########## Boost set up
#include_directories("./boost_1_77_0")
set(BOOST_ROOT boost_1_77_0)
#set(Boost_NO_WARN_NEW_VERSIONS ON)
set(Boost_NO_SYSTEM_PATH ON)
set(Boost_USE_STATIC_LIBS ON)
#find_package(Boost 1.77.0)
# Need to install boost from the system or compile
# program_options separately
find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

########## COMPILE MACROs
#add_definitions(-DMULTITHREAD_DEBUG)
#add_definitions(-DMTD=1)

########## COMPILE libraries
add_executable(GraphColoring main.cpp
        Graph/Algorithms.cpp
        Graph/Graph.h
        Graph/GraphCSR.cpp Graph/GraphAdjL.cpp Graph/GraphAdjM.cpp
        PrintMenu.h)

target_link_libraries(GraphColoring boost_program_options pthread)