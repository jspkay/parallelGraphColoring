//
// Created by salvo on 26/08/21.
//

#ifndef P2_GRAPH_SIMPLE_H
#define P2_GRAPH_SIMPLE_H

#include "./Graph.h"
#include <forward_list>
#include <vector>
#include <stdexcept>

class node;

// IMPORTANT: https://devblogs.microsoft.com/cppblog/the-performance-benefits-of-final-classes/

// list implementation
class Graph_simple : public Graph {
private:
    std::vector<std::forward_list<int>> verteces;
    unsigned long int n;

public:
    Graph_simple(unsigned long int V, unsigned long E); // vertex number
    void addEdge(int a, int b) final;

    void forEachVertex(std::function<void(int)> f) final;
    void forEachNeighbor(int v, std::function<void(int)> f) final;

    //std::vector<int> getNeighbors(int n) final;
};

#endif //P2_GRAPH_SIMPLE_H
