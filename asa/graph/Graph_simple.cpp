//
// Created by salvo on 26/08/21.
//

#include "./Graph.h"
#include "Graph_simple.h"

Graph_simple::Graph_simple(unsigned long V, unsigned long E) :
 Graph<Graph_simple>(V,E) {
    verteces = std::vector<std::forward_list<int>>(V);
    for(auto &el : verteces) el = std::forward_list<int>{};
}

void Graph_simple::addEdge(int a, int b) {
    if(a == b){
        std::cout << " No self loop!\nexiting ";
        exit(-2);
    }

    int m = a > b ? a : b + 1;
    if(m > n){
        std::cout << "The graph only contains " << n << " verteces.\n exiting.";
        exit(-1);
    }

    // Check if the edge exist already
    for(auto &el : verteces[a]){
        if(el == b) return;
    }

    verteces[a].emplace_front(b);
    verteces[b].emplace_front(a);
}


void Graph_simple::forEachVertex(std::function<void(int)> f){
    std::for_each(verteces.begin(), verteces.end(), f);
}

void Graph_simple::forEachNeighbor(int v, std::function<void(int)> f) {
    std::for_each(verteces[v].begin(), verteces[v].end(), f);
}

/*
 * TODO: understand if this method is useful!
std::vector<int> graph::getNeighbors(int n){

    //return std::vector<int>{verteces[n].begin(), verteces[n].end()};

    std::vector<int> res{};

    for(auto el : verteces[n])
        res.push_back(el);

    return res;
}*/