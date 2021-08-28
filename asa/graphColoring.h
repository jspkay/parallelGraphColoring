//
// Created by salvo on 26/08/21.
//

#ifndef P2_GRAPHCOLORING_H
#define P2_GRAPHCOLORING_H

#include "Graph.h"

namespace asa {

    // constants
    enum type_t{ SEQUENTIAL };

    //classes
    class graphColoring {
        std::vector<int> colors;
        std::unique_ptr< std::vector<int> > randoms;
        Graph * g; // TODO: Can we use something else instead of a simple pointer?

    public:
        graphColoring(Graph *g);
        ~graphColoring();

        void sequentialAlgorithm();
        void largestDegree();

        void setRandoms(int maxValue);

        std::vector<int> getColors();
    };

}


#endif //P2_GRAPHCOLORING_H
