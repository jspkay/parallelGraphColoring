#include <iostream>
#include <thread>
#include "asa/graph/Graph_simple.h"
#include "asa/Input.h"
#include "asa/graphColoring.h"
#include "asa/graph/graph_csr.h"

#define GRAPH_TYPE Graph_csr
using namespace asa;

ostream & operator<<(ostream& o, const vector<int> &v){
    int i = 0;
    for(auto& el : v){
        o << i++ << ": " << el << '\n';
    }
    return o;
}

int main() {
    const clock_t begin_time = clock();
    // TODO: Si può sostituire auto?
    auto myGraph = Input<GRAPH_TYPE>::readInput("../asa/graph/benchmark/rgg_n_2_15_s0.txt");

    std::cout << "Time needed to create the graph without coloring " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;

    graphColoring gc(&myGraph);
    gc.sequentialAlgorithm();
    //gc.setRandoms(1000);
    //gc.largestDegree();
    cout << "Colors: " << gc.getColors();

    //mygraph.sequential();
    //mygraph.largestDegree();

    EXIT_SUCCESS;
}
