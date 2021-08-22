//
// Created by salvo on 11/08/21.
//

#include "Graph.h"
#include "fstream"
#include "iostream"
#include <regex>

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
    //fstream fout("output.txt", ios::out);
    string rigaLetta, parsed;
    smatch m;
    int neighbour, length;
    regex code("[0-9]+[ ]*");
    for(int i=0; i<=V; i++){
        getline(fin, rigaLetta);
        while(!rigaLetta.empty()) {
            regex_search(rigaLetta, m, code);
            parsed = m.str();
            length = parsed.size();
            if(parsed.back()==' ')
                parsed.pop_back();
            neighbour = atoi(parsed.c_str());
            std::pair<int, int> edge(i,neighbour);
            edges.emplace_back(edge);
            rigaLetta = rigaLetta.substr(length,rigaLetta.size());
            //fout << edge.first << "->" <<edge.second << "  ";
        }
        //fout << "\n";
    }
    fin.close();
    this->graphCSR = GraphCSR(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V+1);
    int cont = 0;    //id (informatico) parte da 0, quando stamperai dovrai fare +1
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
        cout << "current vertex:" << graphCSR[current_vertex].id << ", neigh -> ";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR){
            cout << graphCSR[neighbor].id << " ";
        }
        cout << "\n";
    }
    return false;
}