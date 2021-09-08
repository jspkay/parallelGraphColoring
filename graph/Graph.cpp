//
// Created by salvo on 11/08/21.
//

#include "Graph.h"
#include "fstream"
#include "iostream"
#include <regex>
#include <thread>

using namespace  std;
#define LEAVE_FREE 1

void Graph::readInput() {
    fstream fin("../graph/benchmark/rgg_n_2_15_s0.txt", ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }
    fin >> V >> E;
    if(!fin.good()) {
        cout << "errore lettura" << endl;
    }

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
    int cont = 0;// degree = 0;    //ci sarà un nodo 0, fittizio
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
        graphCSR[current_vertex].id = cont++;
        graphCSR[current_vertex].color = -1;
        graphCSR[current_vertex].random = rand() % 1000 + 1; //1-1000
        graphCSR[current_vertex].num_iteration = 0; //servirà per jp alg
    }
    concurentThreadsAvailable = std::thread::hardware_concurrency() - LEAVE_FREE;
    active_threads = concurentThreadsAvailable;
    cout << "Fine costruzione grafo in formato CSR!\n";
    cout << "******************\n";
    cout << "V:" << V << ", E:" << E;
    cout << "\n******************\n";
    cout << concurentThreadsAvailable << " core available" << std::endl;
}

void Graph::clearGraph() {
    toColor_set.clear();
    total_set.clear();
    threads.clear();
    active_threads = concurentThreadsAvailable;
    isEnded = false;
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
            graphCSR[current_vertex].color = -1;
            graphCSR[current_vertex].num_iteration = 0;
        }
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
    printOutput("sequential-output.txt");
}

void Graph::printOutput(char* name) {
    fstream fout(name, ios::out);
    if(!fout.is_open()) {
        cout << "errore apertura file fout" << endl;
    }
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        if(graphCSR[current_vertex].id==0)
            continue;
        fout << "u:" << graphCSR[current_vertex].id << ", color: " << static_cast<int>(graphCSR[current_vertex].color) << ", rand:" << graphCSR[current_vertex].random << ", degree:" << boost::out_degree(current_vertex, graphCSR) << ", neigh -> ";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
            fout << "(" << graphCSR[neighbor].id << "," << static_cast<int>(graphCSR[neighbor].color) << ") ";
        }
        fout << "\n";
    }
}

void Graph::largestDegree(){
    //riempio set
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        total_set.push_back(current_vertex);
    }
    //cout << "main thread " << std::this_thread::get_id() << " avviato!"<< endl;
    ///////////////////////////////////////////////////////////////////////////
    for(int n=0; n<concurentThreadsAvailable-1; n++){
        threads.emplace_back([this](){
            //cout << "thread " << std::this_thread::get_id() << " avviato!"<< endl;
            GraphCSR::vertex_descriptor current_vertex;
            bool major = true;
            while (true) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                //////////////////////////////////////////////////////////////////////
                //terminazione thread
                if(total_set.size()==0){
                    active_threads--;
                    if(active_threads==1){
                        isEnded = true;
                        cv.notify_one();
                    }
                    break;
                }
                current_vertex = total_set.front();
                total_set.pop_front();
                ulk.unlock();
                //////////////////////////////////////////////////////////////////////
                std::shared_lock<std::shared_timed_mutex> slk(mutex);
                //cout << std::this_thread::get_id() << ": ottenuto shared" << endl;
                BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
                        if(graphCSR[neighbor].color != -1)
                            continue; //SE GIÀ COLORATO SKIP
                        if (boost::out_degree(current_vertex, graphCSR) < boost::out_degree(neighbor, graphCSR)) {
                            major = false;
                            break;
                        } else if (boost::out_degree(current_vertex, graphCSR) == boost::out_degree(neighbor, graphCSR))
                            if (graphCSR[current_vertex].random < graphCSR[neighbor].random) {
                                major = false;  //A PARITA' DI DEGREE VEDO RANDOM
                                break;
                            }
                }
                slk.unlock();
                //////////////////////////////////////////////////////////////////////
                //cout << std::this_thread::get_id() << ": rilasciato shared" << endl;
                ulk.lock();
                //aggiungo a vertici da colorare se maggiore
                if(major) {
                    toColor_set.push_back(current_vertex);
                    //cout << "size: " << total_set.size() << endl;
                }
                else {
                    total_set.push_back(current_vertex); //reinserisco in coda se non è stato colorato
                    major=true;
                }
                cv.notify_all();
            }
        });
    }
    /////////////////////////////////////////////////////////////////////////////
    GraphCSR::vertex_descriptor current_vertex;
    int C[256]{}, i;
    while(true) {
        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
        //cout << "--------->ottenuto unique" << endl;
        cv.wait(ulk, [this]() { return toColor_set.size() != 0 || isEnded == true; });
        if(isEnded)
            break;
        current_vertex = toColor_set.front();
        toColor_set.pop_front();
        //////////////////////////////////////////////////////////////////////////
        int8_t color = -1;
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
            if (graphCSR[neighbor].color != -1) { //se non colorato, confronto con il nodo corrente
                    C[graphCSR[neighbor].color] = 1;
                }
        }
        for (i = 0; i < 256; i++) {
            if (C[i] == 0 && color == -1) //colore non usato
                color = i;
            else
                C[i] = 0; //reset colori per il prossimo ciclo
        }
        graphCSR[current_vertex].color = color; //coloro il vertice corrente
        //cout << ">---------fine unique" << endl;
    }
    for(std::thread& t : threads)
        t.join();
    printOutput("largestDegree-output.txt");
}


 void Graph::JonesPlassmann(){
    //riempio set
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        total_set.push_back(current_vertex);
    }
    //cout << "main thread " << std::this_thread::get_id() << " avviato!"<< endl;
    ///////////////////////////////////////////////////////////////////////////
    for(int n=0; n<concurentThreadsAvailable-1; n++){
        threads.emplace_back([this](){
            //cout << "thread " << std::this_thread::get_id() << " avviato!"<< endl;
            GraphCSR::vertex_descriptor current_vertex;
            bool major = true;
            while (true) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                //////////////////////////////////////////////////////////////////////
                //terminazione thread
                if(total_set.size()==0){
                    active_threads--;
                    if(active_threads==1){
                        isEnded = true;
                        cv.notify_one();
                    }
                    break;
                }
                current_vertex = total_set.front();
                total_set.pop_front();
                ulk.unlock();
                //////////////////////////////////////////////////////////////////////
                std::shared_lock<std::shared_timed_mutex> slk(mutex);
                //cout << std::this_thread::get_id() << ": ottenuto shared" << endl;
                BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
                        if(graphCSR[neighbor].color != -1)
                            continue; //SE GIÀ COLORATO SKIP
                        if (graphCSR[current_vertex].random < graphCSR[neighbor].random) {
                            major = false;
                            break;
                        }
                        else if(graphCSR[current_vertex].random == graphCSR[neighbor].random){
                            if(current_vertex < neighbor) {
                                major = false;
                                break;
                            }
                        }
                }
                slk.unlock();
                //////////////////////////////////////////////////////////////////////
                //cout << std::this_thread::get_id() << ": rilasciato shared" << endl;
                ulk.lock();
                //aggiungo a vertici da colorare se maggiore
                if(major) {
                    toColor_set.push_back(current_vertex);
                }
                else {
                    graphCSR[current_vertex].num_iteration++;
                    total_set.push_back(current_vertex); //reinserisco in coda se non è stato colorato
                    major=true;
                }
                cv.notify_all();
            }
        });
    }
    /////////////////////////////////////////////////////////////////////////////
    GraphCSR::vertex_descriptor current_vertex;
    int C[256]{}, i;
    while(true) {
        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
        //cout << "--------->ottenuto unique" << endl;
        cv.wait(ulk, [this]() { return toColor_set.size() != 0 || isEnded == true; });
        if(isEnded)
            break;
        current_vertex = toColor_set.front();
        toColor_set.pop_front();
        //////////////////////////////////////////////////////////////////////////
        graphCSR[current_vertex].color =  graphCSR[current_vertex].num_iteration; //coloro il vertice corrente
        //cout << ">---------fine unique" << endl;
    }
    for(std::thread& t : threads)
        t.join();
    printOutput("jp-output.txt");
}




/*void Graph::largestDegree(){
    bool major = true;
    int C[256]{}, i, ci;
    deque<GraphCSR::vertex_descriptor> set;
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
            set.push_back(current_vertex);
        }
    GraphCSR::vertex_descriptor current_vertex;
    while(set.size()!=0){
        current_vertex = set.front();
        //cout << current_vertex << ": " << "\n";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
                //cout << "(" << boost::out_degree(current_vertex, graphCSR) << "," << graphCSR[current_vertex].random << ") vs (" << boost::out_degree(neighbor, graphCSR) << "," << graphCSR[neighbor].random << ")\n";
                if(graphCSR[neighbor].color == -1) { //se non colorato, confronto con il nodo corrente
                    if (boost::out_degree(current_vertex, graphCSR) < boost::out_degree(neighbor, graphCSR)) {
                        major = false;
                        break;
                    } else if (boost::out_degree(current_vertex, graphCSR) == boost::out_degree(neighbor, graphCSR))
                        if (graphCSR[current_vertex].random < graphCSR[neighbor].random) {
                            major = false;
                            break;
                        }
                }
                else //altrimento aggiungo il colore a quelli "già usati"
                    C[graphCSR[neighbor].color] = 1;
            }
        if(major){
            if (boost::out_degree(current_vertex, graphCSR) > 0) {
                int8_t color = -1;
                for (i = 0; i < 256; i++) {
                    if (C[i] == 0 && color == -1) //colore non usato
                        color = i;
                    else
                        C[i] = 0; //reset colori per il prossimo ciclo
                }
                graphCSR[current_vertex].color = color; //coloro il vertice corrente
            }
            else //se non ha vicini, gli do direttamente 0
                graphCSR[current_vertex].color = 0;

            set.pop_front();
        }
        else {
            //se vertice non è stato colorato e lo deve essere, lo reinserisco
            set.pop_front();
            GraphCSR::vertex_descriptor last = set.back();
            set.push_back(current_vertex);
            //reset strutture dati prossimo ciclo
            major = true;
            for(int i=0; i<256; i++){
                C[i]=0; //colore non usato
            }
        }
    }
    printOutput("largest-output.txt");
}*/
