//
// Created by salvo on 11/08/21.
//

#include "Graph.h"
#include "fstream"
#include "iostream"
using namespace  std;

Graph::Graph() {
    //constexpr auto mode = ios::in | ios::binary;
    fstream fin("../graph/benchmark/rgg_n_2_15_s0.txt", ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }
    fin >> V >> E;
    if(!fin.good()) {
        cout << "errore lettura" << endl;
    }
    for(int i=1; i<=V; i++){
        while(1) {
            int neighbour;
            fin >> neighbour;
            std::pair<int, int> edge(i,neighbour);
            //cout << edge.first << " " << edge.second << endl;
            edges.emplace_back(edge);
            if(fin.get()=='\n' || fin.eof())
                break;
        }
    }
    fin.close();
    this->graphCSR = GraphCSR(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V+1); //!!!!
    int cont = 1;
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
            graphCSR[current_vertex].id = cont++;
            graphCSR[current_vertex].color = UINT8_MAX;
        }
    cout << "Fine costruzione grafo in formato CSR!\n";
    cout << "******************\n";
    cout << "V:" << V << ", E:" << E;
    cout << "\n******************" << std::endl;
}

bool Graph::doColoring() {
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR){
            cout << "u:" << graphCSR[current_vertex].id << " ,v:" << graphCSR[neighbor].id << "\n" << endl;
        }
        /*for (auto [neighbor, end] = boost::adjacent_vertices(current_vertex, graphCSR); neighbor != end; ++neighbor) {

        }
         */
    }
    return false;
}