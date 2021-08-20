//
// Created by salvo on 11/08/21.
//

#ifndef P2_GRAPH_H
#define P2_GRAPH_H

#include <forward_list>
#include <vector>
#include <stdexcept>

struct VertexProps { int id; uint8_t color; };

// list implementation
class Graph {
private:
    int V, E;  //number Verteces and Edges
    std::vector<std::pair<int, int>> edges;

    /*std::vector<std::forward_list<int>> verteces;
    unsigned long int n;
    */

public:
    Graph();
    /*graph(unsigned long int n); // vertex number
    void addEdge(int a, int b);
    std::vector<int> getNeighbors(int n);*/
};


#endif //P2_GRAPH_H
