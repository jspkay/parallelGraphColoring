//
// Created by salvo on 25/08/21.
//

#ifndef P2_GRAPH_CSR_H
#define P2_GRAPH_CSR_H

#include "Graph.h"

struct vertexDescriptor_t { int id,random/*,degree; uso boost::out_degree*/; int8_t color; }; //change from int to int8_t
typedef boost::compressed_sparse_row_graph<boost::bidirectionalS, vertexDescriptor_t> graphCsr_t;

class graph_csr : public graph{
    std::vector<std::pair<int, int>> edges;
    graphCsr_t g;

    //void addEdge(int a, int b);
};


#endif //P2_GRAPH_CSR_H
