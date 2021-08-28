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


namespace asa {
    typedef unsigned long node;

    class Graph {
    protected:
        long unsigned V, E;

    public:

        // destructor
        virtual ~Graph() = default;;

        // graph construction
        Graph(unsigned long V, unsigned long E) :
                V(V), E(E) {};
        virtual void addEdge(node a, node b) = 0;
        virtual void prepare() = 0;

        // methods
        unsigned getSize() const {
            return V;
        }

        virtual void forEachVertex(std::function<void(unsigned long)> f) = 0;
        virtual void forEachNeighbor(unsigned long v, std::function<void(unsigned long)> f) = 0;

        virtual unsigned long getDegree(node v) = 0;
    };

}

struct vertexDescriptor_t { int id,random/*,degree; uso boost::out_degree*/; int8_t color; }; //change from int to int8_t
typedef boost::compressed_sparse_row_graph<boost::bidirectionalS, vertexDescriptor_t> GraphCSR;
class Graph {
private:
    int V, E;  //number Verteces and Edges
    std::vector<std::pair<int, int>> edges;
    GraphCSR graphCSR;
    void readInput();
    void printOutput(char* name);
    unsigned concurrentThreadsSupported;
public:
    Graph();
    void sequential();
    void largestDegree();
};


#endif //P2_GRAPH_H
