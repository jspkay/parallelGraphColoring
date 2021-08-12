#include <iostream>
#include "graph/graph.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    graph a = graph(10);

    a.addEdge(12, 15);
//    a.addEdge(1,2);
    a.addEdge(2,3);
    a.addEdge(3,4);
    return 0;
}
