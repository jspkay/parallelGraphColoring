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
#include <mutex>
#include <condition_variable>
#include <thread>
#include <shared_mutex>


struct VertexDescriptor { int id,random; int8_t color; }; //change from int to int8_t
typedef boost::compressed_sparse_row_graph<boost::bidirectionalS,VertexDescriptor> GraphCSR;

class Graph {
private:
    int V, E;  //number Verteces and Edges
    std::vector<std::pair<int, int>> edges;
    GraphCSR graphCSR;
    void readInput();
    void printOutput(char* name);
    unsigned concurentThreadsAvailable;
    std::shared_timed_mutex mutex;
    std::condition_variable_any cv;
    std::deque<GraphCSR::vertex_descriptor> total_set, toColor_set;
    std::vector<std::thread> threads;
    int active_threads;
    bool isEnded = false;
public:
    Graph();
    void sequential();
    void largestDegree();
    void JonesPlassmann();
};


#endif //P2_GRAPH_H
