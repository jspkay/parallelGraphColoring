//
// Created by salvo on 25/08/21.
//

#ifndef P2_GRAPH_CSR_H
#define P2_GRAPH_CSR_H

#include "../Graph.h"

namespace asa {
    struct vertexDescriptor_t {
        int id; // random/*,degree; uso boost::out_degree*/;
        //int8_t color;
    }; //change from int to int8_t
    typedef boost::compressed_sparse_row_graph<boost::bidirectionalS, vertexDescriptor_t> graph_csr_t;


    class Graph_csr : public Graph {
        std::vector<std::pair<int, int>> edges;
        graph_csr_t g;

    public:
        Graph_csr(unsigned long V, unsigned long E);
        void addEdge(unsigned long a, unsigned long b) final;
        void prepare() final;

        void forEachVertex(std::function<void(unsigned long)> f) final;
        void forEachNeighbor(unsigned long v, std::function<void(unsigned long)> f) final;
        unsigned long getDegree(unsigned long V) final;
    };

}


#endif //P2_GRAPH_CSR_H
