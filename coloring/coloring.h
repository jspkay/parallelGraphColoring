//
// Created by salvo on 20/08/21.
//

#ifndef P2_COLORING_H
#define P2_COLORING_H

#include "../graph/graph.h"

class coloring {
    graph G;
    std::vector<int> colors;


public:
    coloring(graph G){
        this->G = G;
    }

    addColor(int vertex, int c){
        colors[vertex] = c;
    }
};


#endif //P2_COLORING_H
