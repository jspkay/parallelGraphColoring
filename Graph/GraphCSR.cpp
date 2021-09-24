//
// Created by antonio_vespa on 13/09/21.
//

#include "Graph.h"
#include "iostream"

using namespace asa;
using namespace  std;
#define LEAVE_FREE 1

void GraphCSR::ResetEachVertex(){
    BGL_FORALL_VERTICES(current_vertex, graph, graphCSR) {
            graph[current_vertex].color = -1;
            graph[current_vertex].num_it = 0;
        }
}

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

GraphCSR::GraphCSR() {
    const clock_t begin_time = clock();
    readInput("../Graph/benchmark/rgg_n_2_15_s0.txt");
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

/*void GraphCSR::sequential(){
    int C[256], i;
    int8_t color=-1;
    for(int i=0; i<256; i++){
        C[i]=0; //colore non usato
    }
    BGL_FORALL_VERTICES(current_vertex, graph, graphCSR){
            //cout << "current vertex:" << graphCSR[current_vertex].id << ", neigh -> ";
            BGL_FORALL_ADJ(current_vertex, neighbor, graph, graphCSR){
                    //cout << graph[neighbor].id << " ";
                    if(graph[neighbor].color != -1)
                        C[graph[neighbor].color] = 1;
                }
            //cout << "\n";
            //ricerca colore minimo non usato
            for(i=0; i<256; i++){
                if(C[i]==0 && color==-1) //colore non usato
                    color=i;
                else
                    C[i]=0; //reset colori per il prossimo ciclo
            }
            graph[current_vertex].color = color; //coloro il vertice corrente
            color = -1;
        }
    printOutput("sequential-output.txt");
}

void GraphCSR::printOutput(std::string&& name) {
    fstream fout(name.c_str(), ios::out);
    if(!fout.is_open()) {
        cout << "errore apertura file fout" << endl;
    }
    BGL_FORALL_VERTICES(current_vertex, graph, graphCSR){
            if(graph[current_vertex].id==0)
                continue;
            fout << "u:" << graph[current_vertex].id << ", color: " <<
                static_cast<int>(graph[current_vertex].color) << ", rand:" <<
                graph[current_vertex].random << ", degree:" <<
                boost::out_degree(current_vertex, graph) << ", neigh -> ";
            BGL_FORALL_ADJ(current_vertex, neighbor, graph, graphCSR) {
                    fout << "(" << graph[neighbor].id << "," << static_cast<int>(graph[neighbor].color) << ") ";
                }
            fout << "\n";
        }
}

void GraphCSR::largestDegree(){
    //riempio set
    BGL_FORALL_VERTICES(current_vertex, graph, graphCSR){
            total_set.push_back(current_vertex);
        }
    //cout << "main thread " << std::this_thread::get_id() << " avviato!"<< endl;
    ///////////////////////////////////////////////////////////////////////////
    for(int n=0; n<concurentThreadsAvailable-1; n++){
        threads.emplace_back([this](){
            //cout << "thread " << std::this_thread::get_id() << " avviato!"<< endl;
            graphCSR::vertex_descriptor current_vertex;
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
                BGL_FORALL_ADJ(current_vertex, neighbor, graph, graphCSR) {
                        if(graph[neighbor].color != -1)
                            continue; //SE GIÀ COLORATO SKIP
                        if (boost::out_degree(current_vertex, graph) < boost::out_degree(neighbor, graph)) {
                            major = false;
                            break;
                        } else if (boost::out_degree(current_vertex, graph) == boost::out_degree(neighbor, graph))
                            if (graph[current_vertex].random < graph[neighbor].random) {
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
    graphCSR::vertex_descriptor current_vertex;
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
        BGL_FORALL_ADJ(current_vertex, neighbor, graph, graphCSR) {
                if (graph[neighbor].color != -1) { //se non colorato, confronto con il nodo corrente
                    C[graph[neighbor].color] = 1;
                }
            }
        for (i = 0; i < 256; i++) {
            if (C[i] == 0 && color == -1) //colore non usato
                color = i;
            else
                C[i] = 0; //reset colori per il prossimo ciclo
        }
        graph[current_vertex].color = color; //coloro il vertice corrente
        //if(graphCSR[current_vertex].color > 11)
        //cout << static_cast<int>(graphCSR[current_vertex].color) << endl;
        //cout << ">---------fine unique" << endl;
    }
    for(std::thread& t : threads)
        t.join();
    printOutput("largestDegree-output.txt");
}

void GraphCSR::JonesPlassmann(){
    //riempio set
    BGL_FORALL_VERTICES(current_vertex, graph, graphCSR){
        total_set.push_back(current_vertex);
    }
    //cout << "main thread " << std::this_thread::get_id() << " avviato!"<< endl;
    ///////////////////////////////////////////////////////////////////////////
    for(int n=0; n<concurentThreadsAvailable-1; n++){
        threads.emplace_back([this](){
            //cout << "thread " << std::this_thread::get_id() << " avviato!"<< endl;
            graphCSR::vertex_descriptor current_vertex;
            bool major = true;
            while (true) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                //////////////////////////////////////////////////////////////////////
                // terminazione thread
                if(total_set.size()==0){
                    active_threads--;
                    if(active_threads==1){
                        isEnded = true;
                        cv.notify_one();
                    }
                    break;
                }
                //////////////////////////////////////////////////////////////////////
                current_vertex = total_set.front();
                total_set.pop_front();
                //SE STO INIZIANDO UN NUOVO GIRO, ALLORA ASPETTO CHE TUTTI PRIMA SIANO COLORATI
                //QUANDO FINISCE DI COLORARE IL NUOVO SET, THREAD CHE COLORA AUMENTA numIteration
                if(graph[current_vertex].num_it > numIteration){
                    increase_numIteration++;
                    cv.notify_all();
                }
                cv.wait(ulk,[this,current_vertex](){ return graph[current_vertex].num_it == numIteration || !increase_numIteration; });
                ulk.unlock();
                //////////////////////////////////////////////////////////////////////
                std::shared_lock<std::shared_timed_mutex> slk(mutex);
                // confronto con i vicini
                BGL_FORALL_ADJ(current_vertex, neighbor, graph, graphCSR) {
                    if(graph[neighbor].color != -1){
                        continue; //ignora
                    }
                    if (graph[current_vertex].random < graph[neighbor].random) {
                        major = false;
                        break;
                    }
                    else if(graph[current_vertex].random == graph[neighbor].random){
                        if(current_vertex < neighbor) {
                            major = false;
                            break;
                        }
                    }
                }
                slk.unlock();
                //////////////////////////////////////////////////////////////////////
                ulk.lock();
                //aggiungo a vertici da colorare se maggiore
                if(major) {
                    toColor_set.push_back(current_vertex);
                }
                else {
                    graph[current_vertex].num_it++;
                    total_set.push_back(current_vertex); //reinserisco in coda se non è stato colorato
                    major=true;
                }
                cv.notify_all();
            }
        });
    }
    /////////////////////////////////////////////////////////////////////////////
    // main thread
    graphCSR::vertex_descriptor current_vertex;
    int C[256]{}, i;
    while(true) {
        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
        //cout << "--------->ottenuto unique" << endl;
        cv.wait(ulk, [this]() { return isEnded == true || increase_numIteration == 2; });
        if(isEnded)
            break;
        //means -> increase_numIteration == true
        while(toColor_set.size()!=0){
            //coloro tutti con stesso colore
            current_vertex = toColor_set.front();
            toColor_set.pop_front();
            //////////////////////////////////////////////////////////////////////////
            int8_t color = -1;
            BGL_FORALL_ADJ(current_vertex, neighbor, graph, graphCSR) {
                if (graph[neighbor].color != -1) { //se non colorato, confronto con il nodo corrente
                    C[graph[neighbor].color] = 1;
                }
            }
            for (i = 0; i < 256; i++) {
                if (C[i] == 0 && color == -1) //colore non usato
                    color = i;
                else
                    C[i] = 0; //reset colori per il prossimo ciclo
            }
            graph[current_vertex].color = color; //coloro il vertice corrente
            //if(graphCSR[current_vertex].color > 10)
            //cout << "jp: " << static_cast<int>(graphCSR[current_vertex].color) << endl;
        }
        increase_numIteration = 0;
        numIteration ++;
        //cout << "rimanenti: " << total_set.size() << endl;
        cv.notify_all();
        //cout << ">---------fine unique" << endl;
    }
    //////////////////////////////////////////////////////////////////////////
    for(std::thread& t : threads)
        t.join();
    printOutput("jp-output.txt");
}*/
