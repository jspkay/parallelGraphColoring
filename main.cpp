#include <iostream>
#include <set>
#include "graph/graph.h"

std::vector<int> greedyAlgorithm(graph &G){
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

    return colors;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vec){
    int i=0;
    for(T &el : vec)
      os << i++ << " - "<<   el << '\n';
    return os;
}

int main() {
    graph a = graph(10);

    a.addEdge(2, 5);
    a.addEdge(1,2);
    a.addEdge(2,3);
    a.addEdge(3,4);
    a.addEdge(2, 4);

    std::vector<int> colors = greedyAlgorithm(a);

    std::cout << colors << std::endl;

    return 0;
}
