//
// Created by salvo on 26/08/21.
//

#include "graphColoring.h"

asa::graphColoring::graphColoring(Graph *g) {
    this->g = std::shared_ptr<Graph>(g);
    colors = std::vector<int>(g->getSize());
    for(int &el : colors) el = -1;
}

void asa::graphColoring::sequentialAlgorithm(){

    Graph &g = *this->g;
    auto &colors = this->colors;
    g.forEachVertex([ &g, &colors](int i){ // for each vertex
        std::set<int> neighColors;

        // find all neighbors color
        g.forEachNeighbor(i, [&neighColors, &colors](int ii){
            if(colors[ii] != -1){
                neighColors.insert(colors[ii]);
            }
        } );

        // and assign the smallest not already assigned to neighbors
        int col = 0;
        for(auto el : neighColors){
            if(el == col) col++;
            else break;
        }
        colors[i] = col;

    } );
}

asa::graphColoring::~graphColoring() {

}
