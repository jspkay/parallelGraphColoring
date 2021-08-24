//
// Created by salvo on 11/08/21.
//

#include "Graph.h"
#include "fstream"
#include "iostream"
#include <regex>
#include <thread>

using namespace  std;

void Graph::readInput() {
    fstream fin("../graph/benchmark/rgg_n_2_15_s0.txt", ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }
    fin >> V >> E;
    if(!fin.good()) {
        cout << "errore lettura" << endl;
    }
    //fstream fout("output.txt", ios::out);
    /*
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
    }*/

    string line;
    stringstream lineStream;
    int neighbour;
    for(int i=0; i<=V; i++){
        getline(fin, line);
        lineStream = stringstream(line);
        while(lineStream >> neighbour)
            edges.emplace_back(std::pair<int, int>(i,neighbour));
    }
    fin.close();
}

Graph::Graph() {
    readInput();
    this->graphCSR = GraphCSR(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V+1);
    int cont = 0, degree = 0;    //ci sarà un nodo 0, fittizio
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
        graphCSR[current_vertex].id = cont++;
        graphCSR[current_vertex].color = -1;
        graphCSR[current_vertex].random = rand() % 1000 + 1; //1-1000
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR){
            degree++;
        }
        graphCSR[current_vertex].degree = degree;
        degree = 0; //reset
    }
    concurentThreadsSupported = std::thread::hardware_concurrency();
    cout << "Fine costruzione grafo in formato CSR!\n";
    cout << "******************\n";
    cout << "V:" << V << ", E:" << E;
    cout << "\n******************" << std::endl;
    cout << concurentThreadsSupported << " core available";
}

void Graph::sequential(){
    int C[256], i;
    int8_t color=-1;
    for(int i=0; i<256; i++){
        C[i]=0; //colore non usato
    }
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        //cout << "current vertex:" << graphCSR[current_vertex].id << ", neigh -> ";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR){
            //cout << graphCSR[neighbor].id << " ";
            if(graphCSR[neighbor].color != -1)
                C[graphCSR[neighbor].color] = 1;
        }
        //cout << "\n";
        //ricerca colore minimo non usato
        for(i=0; i<256; i++){
            if(C[i]==0 && color==-1) //colore non usato
                color=i;
            else
                C[i]=0; //reset colori per il prossimo ciclo
        }
        graphCSR[current_vertex].color = color; //coloro il vertice corrente
        color = -1;
    }
    printOutput();
}

void Graph::printOutput() {
    fstream fout("output.txt", ios::out);
    if(!fout.is_open()) {
        cout << "errore apertura file fout" << endl;
    }
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        if(graphCSR[current_vertex].id==0)
            continue;
        fout << "u:" << graphCSR[current_vertex].id << ", color: " << static_cast<int>(graphCSR[current_vertex].color) << ", rand:" << graphCSR[current_vertex].random << ", degree:" << graphCSR[current_vertex].degree << ", neigh -> ";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
            fout << "(" << graphCSR[neighbor].id << "," << static_cast<int>(graphCSR[neighbor].color) << ") ";
        }
        fout << "\n";
    }
}

void Graph::largestDegree(){
    bool major = false;
    int8_t color=-1;
    int C[256], i;
    for(int i=0; i<256; i++){
        C[i]=0; //colore non usato
    }
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {  //qui ci vuole while
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
            if(graphCSR[current_vertex].degree < graphCSR[neighbor].degree)
                break;
            else if(graphCSR[current_vertex].degree == graphCSR[neighbor].degree)
                if(graphCSR[current_vertex].random == graphCSR[neighbor].random)
                    break;
            C[graphCSR[neighbor].color] = 1;
            major = true;
        }
        if(major){
            for(i=0; i<256; i++){
                if(C[i]==0 && color==-1) //colore non usato
                    color=i;
                else
                    C[i]=0; //reset colori per il prossimo ciclo
            }
            graphCSR[current_vertex].color = color; //coloro il vertice corrente
            color = -1;
        }
    }
}
