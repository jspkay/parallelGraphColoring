//
// Created by salvo on 11/08/21.
//

#include "graph.h"

graph::graph() {
    // verteces call default constructor
    n = 0;
}

graph::graph(unsigned long int n) {
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