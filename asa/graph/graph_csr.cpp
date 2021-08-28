//
// Created by salvo on 25/08/21.
//

#include "graph_csr.h"

using asa::Graph_csr;

Graph_csr::Graph_csr(unsigned long V, unsigned long E) :
        Graph(V, E){

}

void Graph_csr::addEdge(unsigned long a, unsigned long b){
    edges.emplace_back(std::pair<int, int>(a, b));
}

void Graph_csr::prepare(){
    g = graph_csr_t(boost::edges_are_unsorted_multi_pass, edges.begin(), edges.end(), V+1);
    int cont=0;
    BGL_FORALL_VERTICES(current_vertex, g, graph_csr_t) {
            g[current_vertex].id = cont++;
    };
}

void asa::Graph_csr::forEachVertex(std::function<void(unsigned long)> f) {
    BGL_FORALL_VERTICES(current_vertex, g, graph_csr_t){
        f(current_vertex);
    }
}

void asa::Graph_csr::forEachNeighbor(unsigned long v, std::function<void(unsigned long)> f) {
    BGL_FORALL_ADJ(v, neighbor, g, graph_csr_t){
        f(neighbor);
    }
}

unsigned long asa::Graph_csr::getDegree(unsigned long v) {
    return boost::out_degree(v, g);
}
