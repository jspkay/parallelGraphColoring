//
// Created by salvo on 25/08/21.
//

#include "graph_csr.h"

void graph_csr::addEdge(int a, int b){
    edges.emplace_back(std::pair<int, int>(a, b));
}

void graph_csr::prepare(){
    g = graph_csr_t(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V+1);
}