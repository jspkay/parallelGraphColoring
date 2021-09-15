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
#include <boost/iostreams/device/mapped_file.hpp>

struct VertexDescriptor { int random/*,degree; uso boost::out_degree*/; int8_t color; }; //change from int to int8_t
typedef boost::compressed_sparse_row_graph<boost::bidirectionalS,VertexDescriptor> GraphCSR;

class Graph {
private:
    int V, E;  //number Verteces and Edges
    std::vector<std::pair<int, int>> edges;
    GraphCSR graphCSR;
    void readInput();
    void printOutput();
    unsigned concurentThreadsSupported;
    std::vector<GraphCSR::vertex_descriptor> set;
public:
    Graph();
    void sequential();
    void largestDegree();
};


#endif //P2_GRAPH_H
