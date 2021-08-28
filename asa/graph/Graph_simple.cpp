//
// Created by salvo on 26/08/21.
//

#include "../Graph.h"
#include "Graph_simple.h"

using asa::Graph_simple;

Graph_simple::Graph_simple(unsigned long V, unsigned long E) : Graph(V,E) {
    verteces = std::vector<std::forward_list<int>>(V);
    for(auto &el : verteces) el = std::forward_list<int>{};
}

void Graph_simple::addEdge(unsigned long a, unsigned long b) {
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

void Graph_simple::prepare() {}


void Graph_simple::forEachVertex(std::function<void(unsigned long)> f){
    int i=0;
    for(auto &el : verteces){
        f(i++);
    }
    //std::for_each(verteces.begin(), verteces.end(), f);
}

void Graph_simple::forEachNeighbor(unsigned long v, std::function<void(unsigned long)> f) {
    std::for_each(verteces[v].begin(), verteces[v].end(), f);
}

unsigned long Graph_simple::getDegree(node v) {
    unsigned long i=0;
    for(auto el : verteces[v]) i++;
    return i;
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