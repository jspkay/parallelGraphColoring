//
// Created by salvo on 11/08/21.
//

#ifndef P2_GRAPH_H
#define P2_GRAPH_H

#include <forward_list>
#include <vector>
#include <stdexcept>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>

class Graph{
    long unsigned V,E;

public:

    // graph construction
    Graph(int V, int E) :
            V(V), E(E){};
    virtual void addEdge(int a, int b) = 0;
    virtual void prepare() = 0;

    // methods
    unsigned getSize() {
        return V;
    }
    virtual void forEachVertex( std::function<void(int)> f) = 0;
    virtual void forEachNeighbor( int v, std::function<void(int)> f) = 0;

    virtual ~Graph() = 0;
};

/*
class Graph {
private:
    int V, E;  //number Verteces and Edges
    std::vector<std::pair<int, int>> edges;
    graphCsr_t graphCSR;
    void readInput();
    void printOutput(char* name);
    unsigned concurrentThreadsSupported;
public:
    Graph();
    void sequential();
    void largestDegree();
};
*/

#endif //P2_GRAPH_H
