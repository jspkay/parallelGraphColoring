#include <iostream>
#include <fstream>
#include "graph/Graph.h"


int main() {
    const clock_t begin_time = clock();
    Graph mygraph = Graph();
    std::cout << "Time needed to create the graph without coloring " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    mygraph.doColoring();



    /*graph a = graph(10);
    a.addEdge(12, 15);
//    a.addEdge(1,2);
    a.addEdge(2,3);
    a.addEdge(3,4);*/
    return 0;
}
