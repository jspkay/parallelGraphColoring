//
// Created by antonio_vespa on 13/09/21.
//

#include "Graph.h"
#include "iostream"

using namespace asa;
using namespace  std;
#define LEAVE_FREE 1

void GraphAdjM::forEachVertex(node* current_vertex, std::function<void()> f){
    BGL_FORALL_VERTICES(curr, graph, graphAdjM){
            *current_vertex = curr;
            f();
        }
}

void GraphAdjM::forEachNeighbor(node current_vertex, node* neighbor, std::function<void()> f){
    BGL_FORALL_ADJ(current_vertex, neigh, graph, graphAdjM){
            *neighbor = neigh;
            f();
        }
}

int GraphAdjM::getDegree(node v) {
    return boost::out_degree(v, graph);
}

GraphAdjM::GraphAdjM(vector<std::pair<node, node>>& edges, int V, int E){
    this->V = V;
    this->E = E;
    graph = graphAdjM(std::begin(edges), edges.end(), V);
    BGL_FORALL_VERTICES(current_vertex, graph, graphAdjM) {
            graph[current_vertex].color = -1;
            graph[current_vertex].num_it = 0;
            graph[current_vertex].random = rand() % 1000 + 1; //1-1000
            graph[current_vertex].toBeDeleted = false;
            graph[current_vertex].weight = -1; //smallest degree alg
        }
    numIteration = 0;
    increase_numIteration = 0;
    //concurrentThreadsActive = std::thread::hardware_concurrency() - LEAVE_FREE;
    //active_threads = concurrentThreadsActive;
    cout << "Fine costruzione grafo in formato ADJ MATRIX!\n";
    cout << "******************\n";
    cout << "V:" << V << ", E:" << E;
    cout << "\n******************\n";
    //cout << concurrentThreadsActive << " core available" << std::endl;
}