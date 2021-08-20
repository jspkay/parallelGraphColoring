//
// Created by salvo on 11/08/21.
//

#include "Graph.h"
#include "fstream"
#include "iostream"
using namespace  std;

Graph::Graph() {
    //constexpr auto mode = ios::in | ios::binary;
    fstream fin("/home/antonio_vespa/Documenti/GitHub/parallelGraphColoring/graph/benchmark/rgg_n_2_15_s0.txt", ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }
    fin >> V >> E;
    if(!fin.good()) {
        cout << "errore lettura" << endl;
    }
    else
        cout << "V:" << V << ", E:" << E << std::endl;
    fin.close();
}

/*graph::graph(unsigned long int n) {
    verteces = std::vector<std::forward_list<int>>{n};
    for(auto &el : verteces) el = std::forward_list<int>{};
    this->n = n;
}

void graph::addEdge(int a, int b) {
    if(a == b){
        return;
    }

    int m = a > b ? a : b + 1;
    if(m > n){
        n = m;
        verteces.resize(m);
    }

    verteces[a].emplace_front(b);
    verteces[b].emplace_front(a);
}
*/