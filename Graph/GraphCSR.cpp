//
// Created by antonio_vespa on 13/09/21.
//

#include "Graph.h"
#include "iostream"

using namespace asa;
using namespace  std;
#define LEAVE_FREE 0

void GraphCSR::forEachVertex(node* current_vertex, std::function<void()> f){
    BGL_FORALL_VERTICES(curr, graph, graphCSR){
            *current_vertex = curr;
            f();
        }
}

void GraphCSR::forEachNeighbor(node current_vertex, node* neighbor, std::function<void()> f){
    BGL_FORALL_ADJ(current_vertex, neigh, graph, graphCSR){
            *neighbor = neigh;
            f();
        }
}

int GraphCSR::getDegree(node v) {
    return boost::out_degree(v, graph);
}

GraphCSR::GraphCSR(string fin_name) {
    const clock_t begin_time = clock();
    readInput(fin_name);
    std::cout << "Time needed to read the graph " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    graph = graphCSR(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V);
    BGL_FORALL_VERTICES(current_vertex, graph, graphCSR) {
            graph[current_vertex].color = -1;
            graph[current_vertex].num_it = 0;
            graph[current_vertex].random = rand() % 1000 + 1; //1-1000
            graph[current_vertex].toBeDeleted = false;
            graph[current_vertex].weight = -1; //smallest degree alg
        }
    numIteration = 0;
    increase_numIteration = 0;
    concurentThreadsAvailable = std::thread::hardware_concurrency() - LEAVE_FREE;
    active_threads = concurentThreadsAvailable;
    cout << "Fine costruzione grafo in formato CSR!\n";
    cout << "******************\n";
    cout << "V:" << V << ", E:" << E;
    cout << "\n******************\n";
    cout << concurentThreadsAvailable << " core available" << std::endl;
}