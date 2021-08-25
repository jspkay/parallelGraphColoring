#include <iostream>
#include <set>
#include "graph/graph.h"

void greedyAlgorithm(graph &G){
    std::vector<int> colors(static_cast<int>(G.getSize()));
    for(int i=0; i<G.getSize(); i++)
        colors[i] = -1;


    for(int i=0; i<G.getSize(); i++){ // for each vertex
        std::set<int> colorNeigh{};
        for(auto &j : G.getNeighbors(i)){
            if(colors[j] != -1)
                colorNeigh.insert( colors[j] );
        }

        int col = 0;
        for(auto &el : colorNeigh){
            if(el == col) col++;
            else break;
        }
        colors[i] = col;
    }


}

int main() {
    graph a = graph(10);

    a.addEdge(2, 5);
    a.addEdge(2, 5);
    a.addEdge(1,2);
    a.addEdge(2,3);
    a.addEdge(3,4);

    greedyAlgorithm(a);

    return 0;
}
