#include <iostream>
#include <fstream>
#include <thread>
#include "graph/Graph.h"


int main() {
    const clock_t begin_time = clock();
    Graph mygraph;
    const clock_t end_construction_time = clock();
    const clock_t begin_seq_time = clock();
    mygraph.sequential();
    const clock_t end_seq_time = clock();
    const clock_t begin_jp_time = clock();
    mygraph.JonesPlassmann();
    const clock_t end_jp_time = clock();
    const clock_t begin_ld_time = clock();
    mygraph.largestDegree();
    const clock_t end_ld_time = clock();
    std::cout << "******************\n";
    std::cout << "Time needed to create the graph without coloring " << float( end_construction_time - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time sequential: " << float( end_seq_time - begin_seq_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time jp: " << float( end_jp_time -begin_jp_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time ld (seq): " << float( end_ld_time -begin_ld_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    EXIT_SUCCESS;
}
