#include <iostream>
#include <fstream>
#include <thread>
#include "asa/graph/Graph.h"


int main() {
    const clock_t begin_time = clock();
    Graph mygraph = Graph();
    std::cout << "Time needed to create the graph without coloring " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    //mygraph.sequential();
    mygraph.largestDegree();
    EXIT_SUCCESS;
}
