//
// Created by salvo on 11/08/21.
//

#include "Graph.h"
#include "fstream"
#include "iostream"
#include <regex>
#include <thread>
#include <queue>
#include <mutex>

using namespace  std;

void Graph::readInput() {
    string line;
    stringstream lineStream;
    fstream fin("../graph/benchmark/rgg_n_2_15_s0.txt", ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }
    getline(fin, line);
    lineStream = stringstream(line);
    lineStream >> V >> E;
    if(!fin.good()) {
        cout << "errore lettura" << endl;
    }

    int neighbour;
    queue<string> q;
    mutex mq;

    //consumer

    auto cons = [&mq, &q, this](){
        string line;
        stringstream lineStream;
        int neighbour;
        int i;
        while(true){
            {
                lock_guard<mutex> lk(mq);
                if(q.empty())
                    continue;
                line = q.front();
                q.pop();
            }
            if(line == "stop")
                return;
            lineStream = stringstream(line);
            lineStream >> i;
            while(lineStream >> neighbour)
                edges.emplace_back(std::pair<int, int>(i,neighbour-1));


        }
    };
    //start consumer
    thread consT(cons);
    //producer
    for(int i=0; i<V; i++){
        getline(fin, line);
        {
            lock_guard<mutex> lk(mq);
            q.emplace(std::to_string(i) + " " + line);
        }
    }
    {
        lock_guard<mutex> lk(mq);
        q.push("stop");
    }
    fin.close();/*
    for(auto a = std::begin(edges); a != std::end(edges); a++)
        cout << a->first << " " << a->second << endl;
*/
    consT.join();

 }

Graph::Graph() {
    readInput();
    this->graphCSR = GraphCSR(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V);
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
        graphCSR[current_vertex].color = -1;
        graphCSR[current_vertex].random = rand() % 1000 + 1; //1-1000
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
    //ricorda di usare current_vertex+1 dato che gli indici partono da 1 nel file, mentre da 0 nella csr
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        fout << "u:" << current_vertex+1 << ", color: " << static_cast<int>(graphCSR[current_vertex].color) << ", rand:" << graphCSR[current_vertex].random << ", degree:" << boost::out_degree(current_vertex, graphCSR) << ", neigh -> ";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
            fout << "(" << neighbor+1 << "," << static_cast<int>(graphCSR[neighbor].color) << ") ";
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
            if(boost::out_degree(current_vertex, graphCSR) < boost::out_degree(neighbor, graphCSR))
                break;
            else if(boost::out_degree(current_vertex, graphCSR) == boost::out_degree(neighbor, graphCSR))
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
