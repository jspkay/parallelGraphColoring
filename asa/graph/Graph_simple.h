//
// Created by salvo on 26/08/21.
//

#ifndef P2_GRAPH_SIMPLE_H
#define P2_GRAPH_SIMPLE_H

#include "../Graph.h"
#include <forward_list>
#include <vector>
#include <stdexcept>

//class node;

// IMPORTANT: https://devblogs.microsoft.com/cppblog/the-performance-benefits-of-final-classes/


namespace asa {

    class Graph_simple final : public Graph {
    private:
        std::vector<std::forward_list<int>> verteces;
        unsigned long int n;

    public:
        Graph_simple(unsigned long V, unsigned long E); // vertex number
        void addEdge(unsigned long a, unsigned long b) final;
        void prepare() final;

        void forEachVertex(std::function<void(unsigned long)> f) final;
        void forEachNeighbor(unsigned long v, std::function<void(unsigned long)> f) final;

        //std::vector<int> getNeighbors(int n) final;
        unsigned long getDegree(node v) final;

        ~Graph_simple() final = default;
    };

}


#endif //P2_GRAPH_SIMPLE_H
