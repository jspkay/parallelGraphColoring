#include <iostream>
#include <fstream>
#include <thread>
#include "graph/Graph.h"


int main() {
    const clock_t begin_time = clock();
    Graph mygraph;
    std::cout << "Time needed to create the graph without coloring " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    //mygraph.sequential();
    mygraph.JonesPlassmann();
    EXIT_SUCCESS;
}
