//
// Created by salvo on 26/08/21.
//

#ifndef P2_GRAPHCOLORING_H
#define P2_GRAPHCOLORING_H

#include "graph/Graph.h"

namespace asa {

    // constants
    enum type_t{ SEQUENTIAL };

    //classes
    class graphColoring {
        std::vector<int> colors;
        std::shared_ptr<Graph> g;

        void sequentialAlgorithm();

    public:
        graphColoring(Graph *g );
        virtual ~graphColoring();

    };

}


#endif //P2_GRAPHCOLORING_H
