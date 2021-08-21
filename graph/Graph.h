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

struct VertexDescriptor { int id; uint8_t color; };
typedef boost::compressed_sparse_row_graph<boost::bidirectionalS,VertexDescriptor> GraphCSR;

// list implementation
class Graph {
private:
    int V, E;  //number Verteces and Edges
    std::vector<std::pair<int, int>> edges;
    GraphCSR graphCSR;


    /*std::vector<std::forward_list<int>> verteces;
    unsigned long int n;
    */

public:
    Graph();
    bool doColoring();
    /*graph(unsigned long int n); // vertex number
    void addEdge(int a, int b);
    std::vector<int> getNeighbors(int n);*/
};


#endif //P2_GRAPH_H
