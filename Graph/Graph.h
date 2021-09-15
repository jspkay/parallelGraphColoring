//
// Created by salvo on 11/08/21.
//

#ifndef P2_GRAPH_H
#define P2_GRAPH_H

#include <functional>
#include <string>
#include <deque>
#include <forward_list>
#include <vector>
#include <stdexcept>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <shared_mutex>

namespace asa {
    struct vertexDescriptor { int id,random,num_it,weight; bool toBeDeleted; int8_t color; };
    typedef unsigned long node;
    typedef boost::compressed_sparse_row_graph<boost::bidirectionalS,vertexDescriptor> graphCSR;
    typedef boost::adjacency_matrix<boost::undirectedS,vertexDescriptor> graphAdjM;
    typedef boost::adjacency_list<boost::listS,boost::vecS,boost::undirectedS,vertexDescriptor> graphAdjL;

    /*** Base class for CRTP ***/
    template <typename T>
    class Graph {
    private:
        friend T;
        Graph(){};
    protected:
        unsigned long V;
        unsigned long E;
        std::vector<std::pair<node, node>> edges;
        unsigned concurentThreadsAvailable;
        /*** variabili per algoritmi di colorazione ***/
        std::shared_timed_mutex mutex;
        std::condition_variable_any cv;
        std::vector<std::thread> threads;
        std::deque<node> total_set, toColor_set;
        int active_threads;
        int numIteration, increase_numIteration;
        bool isEnded = false;
    public:
        void readInput(std::string&& fname){
            std::fstream fin(fname, std::ios::in);
            if(!fin.is_open()) {
                std::cerr << "errore apertura file fin" << std::endl;
            }
            fin >> V >> E;
            if(!fin.good()) {
                std::cerr << "errore lettura" << std::endl;
            }
            std::string line;
            std::stringstream lineStream;
            int neighbour;
            /***  evito riallocazioni dinamiche multiple ***/
            edges.reserve(E); //riservo E posti,
            total_set.resize(V); //riservo V posti
            toColor_set.resize(V/4);
            /***/
            for(int i=0; i <= V; i++){
                getline(fin, line);
                lineStream = std::stringstream(line);
                while(lineStream >> neighbour)
                    edges.emplace_back(std::pair<int, int>(i,neighbour));
            }
            fin.close();
        };
        void clearGraph(){
            toColor_set.clear();
            total_set.clear();
            threads.clear();
            active_threads = concurentThreadsAvailable;
            isEnded = false;
            ResetEachVertex();
            /***  evito riallocazioni dinamiche multiple ***/
            total_set.resize(V); //riservo V posti
            toColor_set.resize(V/4);
            /***/
        };
        void sequential(){
            int C[256];
            int8_t color=-1;
            for(int i=0; i<256; i++){
                C[i]=0; //colore non usato
            }
            node current_vertex;
            forEachVertex(&current_vertex,[this,&C,&color,&current_vertex](){
                int i,lastColorFound=0;
                node neighbor;
                forEachNeighbor(current_vertex,&neighbor,[this,&current_vertex,&neighbor,&C, &lastColorFound](){
                    if(static_cast<T&>(*this).graph[neighbor].color != -1) {
                        C[static_cast<T &>(*this).graph[neighbor].color] = 1;
                        if(static_cast<T&>(*this).graph[neighbor].color > lastColorFound)
                            lastColorFound = static_cast<T&>(*this).graph[neighbor].color;
                    }
                });
                for(i=0; i<=lastColorFound + 1; i++){
                    if(C[i]==0 && color==-1) //colore non usato
                        color=i;
                    else
                        C[i]=0; //reset colori per il prossimo ciclo
                }
                static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                color = -1;
            });
            printOutput("sequential-output.txt");
        };
        void printOutput(std::string&& name) {
            std::fstream fout(name.c_str(), std::ios::out);
            if (!fout.is_open()) {
                std::cerr << "errore apertura file fout\n";
            }
            node current_vertex;
            forEachVertex(&current_vertex,[this,&current_vertex,&fout](){
                if (static_cast<T&>(*this).graph[current_vertex].id!= 0) {
                    fout << "u:" << static_cast<T&>(*this).graph[current_vertex].id << ", color: " <<
                         static_cast<int>(static_cast<T&>(*this).graph[current_vertex].color) << ", rand:" << static_cast<T&>(*this).graph[current_vertex].random << ", degree:" <<
                         boost::out_degree(current_vertex, static_cast<T&>(*this).graph) << ", weight: " << static_cast<T&>(*this).graph[current_vertex].weight << ", neigh -> ";
                    node neighbor;
                    forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &fout]() {
                        fout << "(" << static_cast<T&>(*this).graph[neighbor].id << "," << static_cast<int>(static_cast<T&>(*this).graph[neighbor].color) << ") ";
                    });
                    fout << "\n";
                }
            });
            fout.close();
        }
        void largestDegree(){
            //riempio set
            node current_vertex;
            forEachVertex(&current_vertex,[this,&current_vertex](){
                total_set.push_back(current_vertex);
            });
            ///////////////////////////////////////////////////////////////////////////
            for(int n=0; n<concurentThreadsAvailable-1; n++){
                threads.emplace_back([this](){
                    //cout << "thread " << std::this_thread::get_id() << " avviato!"<< endl;
                    node current_vertex;
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
                        node neighbor;
                        forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, current_vertex, &major]() {
                            if(static_cast<T&>(*this).graph[neighbor].color == -1) {
                                if (getDegree(current_vertex) < getDegree(neighbor)) {
                                    major = false;
                                    return;
                                } else if (getDegree(current_vertex) == getDegree(neighbor))
                                    if (static_cast<T&>(*this).graph[current_vertex].random < static_cast<T&>(*this).graph[neighbor].random) {
                                        major = false;  //A PARITA' DI DEGREE VEDO RANDOM
                                        return;
                                    }
                            }
                        });
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
                int8_t color = -1, lastColorFound=0;
                node neighbor;
                forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &C, &lastColorFound]() {
                    if (static_cast<T&>(*this).graph[neighbor].color != -1) { //se non colorato, confronto con il nodo corrente
                        C[static_cast<T&>(*this).graph[neighbor].color] = 1;
                        if(static_cast<T&>(*this).graph[neighbor].color > lastColorFound)
                            lastColorFound = static_cast<T&>(*this).graph[neighbor].color;
                    }
                });
                for (i = 0; i <= lastColorFound + 1; i++) {
                    if (C[i] == 0 && color == -1) //colore non usato
                        color = i;
                    else
                        C[i] = 0; //reset colori per il prossimo ciclo
                }
                static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                //cout << ">---------fine unique" << endl;
            }
            for(std::thread& t : threads)
                t.join();
            printOutput("largestDegree-output.txt");
        };
        void JonesPlassmann(){
            //riempio set
            node current_vertex;
            forEachVertex(&current_vertex,[this,&current_vertex](){
                total_set.push_back(current_vertex);
            });
            ///////////////////////////////////////////////////////////////////////////
            for(int n=0; n<concurentThreadsAvailable-1; n++){
                threads.emplace_back([this](){
                    //cout << "thread " << std::this_thread::get_id() << " avviato!"<< endl;
                    node current_vertex;
                    bool major = true;
                    while (true) {
                        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                        //////////////////////////////////////////////////////////////////////
                        /*** terminazione thread ***/
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
                        if(static_cast<T&>(*this).graph[current_vertex].num_it > numIteration){
                            increase_numIteration++;
                            cv.notify_all();
                        }
                        cv.wait(ulk,[this,current_vertex](){ return static_cast<T&>(*this).graph[current_vertex].num_it == numIteration || !increase_numIteration; });
                        ulk.unlock();
                        //////////////////////////////////////////////////////////////////////
                        std::shared_lock<std::shared_timed_mutex> slk(mutex);
                        /*** confronto con i vicini ***/
                        node neighbor;
                        forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, current_vertex, &major]() {
                            if(static_cast<T&>(*this).graph[neighbor].color == -1) {
                                if (static_cast<T&>(*this).graph[current_vertex].random < static_cast<T&>(*this).graph[neighbor].random) {
                                    major = false;
                                    return;
                                }
                                else if(static_cast<T&>(*this).graph[current_vertex].random == static_cast<T&>(*this).graph[neighbor].random){
                                    if(current_vertex < neighbor) {
                                        major = false;
                                        return;
                                    }
                                }
                            }
                        });
                        slk.unlock();
                        //////////////////////////////////////////////////////////////////////
                        ulk.lock();
                        //aggiungo a vertici da colorare se maggiore
                        if(major) {
                            toColor_set.push_back(current_vertex);
                        }
                        else {
                            static_cast<T&>(*this).graph[current_vertex].num_it++;
                            total_set.push_back(current_vertex); //reinserisco in coda se non è stato colorato
                            major=true;
                        }
                        cv.notify_all();
                    }
                });
            }
            /////////////////////////////////////////////////////////////////////////////
            /*** main thread ***/
            int C[256]{}, i;
            while(true) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                //cout << "--------->ottenuto unique" << endl;
                cv.wait(ulk, [this]() { return isEnded == true || increase_numIteration == concurentThreadsAvailable - 1; });
                if(isEnded)
                    break;
                //means -> increase_numIteration == true
                while(toColor_set.size()!=0){
                    //coloro tutti con stesso colore
                    current_vertex = toColor_set.front();
                    toColor_set.pop_front();
                    //////////////////////////////////////////////////////////////////////////
                    int8_t color = -1, lastColorFound = 0;
                    node neighbor;
                    forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &C, &lastColorFound](){
                        if (static_cast<T&>(*this).graph[neighbor].color != -1) { //se non colorato, confronto con il nodo corrente
                            C[static_cast<T&>(*this).graph[neighbor].color] = 1;
                            if(static_cast<T&>(*this).graph[neighbor].color > lastColorFound)
                                lastColorFound = static_cast<T&>(*this).graph[neighbor].color;
                        }
                    });
                    for (i = 0; i <= lastColorFound + 1; i++) {
                        if (C[i] == 0 && color == -1) //colore non usato
                            color = i;
                        else
                            C[i] = 0; //reset colori per il prossimo ciclo
                    }
                    static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
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
        };
        void smallestDegree(){
            /*** weighting phase ***/
            //k = numIteration - i = current_color
            //riempio set
            node current_vertex;
            numIteration = 1;
            int current_weigth = 0;
            forEachVertex(&current_vertex,[this,&current_vertex](){
                total_set.push_back(current_vertex);
            });
            ///////////////////////////////////////////////////////////////////////////
            for(int n=0; n<concurentThreadsAvailable-1; n++){
                threads.emplace_back([this](){
                    //std::cout << "thread " << std::this_thread::get_id() << " avviato!" << std::endl;
                    node current_vertex;
                    bool minor = false;
                    while (true) {
                        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                        //////////////////////////////////////////////////////////////////////
                        /*** terminazione thread ***/
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
                        if(static_cast<T&>(*this).graph[current_vertex].num_it > numIteration){
                            increase_numIteration++;
                            cv.notify_all();
                        }
                        cv.wait(ulk,[this,current_vertex](){ return static_cast<T&>(*this).graph[current_vertex].num_it == numIteration || !increase_numIteration; });
                        ulk.unlock();
                        //////////////////////////////////////////////////////////////////////
                        std::shared_lock<std::shared_timed_mutex> slk(mutex);
                        int degreeCurrVertex = 0;
                        node neighbor;
                        /*** calcolo degree current_vertex ***/
                        forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &degreeCurrVertex](){
                            if(!static_cast<T&>(*this).graph[neighbor].toBeDeleted)
                                degreeCurrVertex++;  //se non è marcato toBeDeleted,aumento degree
                        });
                        std::cout << degreeCurrVertex << std::endl;
                        /*** current_vertex ha smallest degree? ***/
                        if (getDegree(current_vertex) <= numIteration)
                            minor = true;
                        slk.unlock();
                        //////////////////////////////////////////////////////////////////////
                        ulk.lock();
                        //aggiungo a vertici da colorare se maggiore
                        if(minor){
                            toColor_set.push_back(current_vertex);
                            //std::cout << "u: " << current_vertex << ", degree: " << getDegree(current_vertex) << ", curr_min: " << current_min_degree << std::endl;
                        }
                        else {
                            static_cast<T&>(*this).graph[current_vertex].num_it++;
                            total_set.push_back(current_vertex); //reinserisco in coda se non è stato colorato
                        }
                        minor = false;
                        cv.notify_all();
                    }
                });
            }
            /////////////////////////////////////////////////////////////////////////////
            /*** main thread ***/
            while(true) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                //cout << "--------->ottenuto unique" << endl;
                cv.wait(ulk, [this]() { return isEnded == true || increase_numIteration == concurentThreadsAvailable - 1; });
                if(isEnded) {
                    std::cout << "fine sd" <<std::endl;
                    break;
                }
                //means -> increase_numIteration == concurentThreadsAvailable - 1
                bool doIHaveToIncreaseColor = toColor_set.size()!=0;
                while(toColor_set.size()!=0){
                    current_vertex = toColor_set.front();
                    toColor_set.pop_front();

                    //////////////////////////////////////////////////////////////////////////
                    static_cast<T&>(*this).graph[current_vertex].weight = current_weigth; //coloro il vertice corrente
                    static_cast<T&>(*this).graph[current_vertex].toBeDeleted = true;
                }
                if(doIHaveToIncreaseColor)
                    current_weigth++;
                increase_numIteration = 0;
                numIteration ++;
                std::cout << "rimanenti: " << total_set.size() << std::endl;
                cv.notify_all();
                //cout << ">---------fine unique" << endl;
            }
            //////////////////////////////////////////////////////////////////////////
            /*** coloring phase ***/
            
            //////////////////////////////////////////////////////////////////////////
            for(std::thread& t : threads)
                t.join();
            printOutput("smallestDegree-output.txt");
        };
        /*** da specializzare in ogni rappresentazione interna ***/
        void forEachVertex(node* current_vertex, std::function<void()> f){
            return static_cast<T&>(*this).forEachVertex(current_vertex,f);
        };
        void ResetEachVertex(){
            return static_cast<T&>(*this).ResetEachVertex();
        };
        void forEachNeighbor(node v, node* neighbor, std::function<void()> f){
            return static_cast<T&>(*this).forEachNeighbor(v,neighbor,f);
        };
        int getDegree(node v){
            return static_cast<T&>(*this).getDegree(v);
        };
    };

    /*** CRTP definitions***/
    /*** 1) CSR Internal representation***/
    class GraphCSR : public Graph<GraphCSR>{
    private:
        friend Graph<GraphCSR>;
        graphCSR graph;
    public:
        GraphCSR();
        /*** specializzazioni ***/
        void ResetEachVertex();
        void forEachVertex(node* current_vertex, std::function<void()> f);
        void forEachNeighbor(node v, node* neighbor, std::function<void()> f);
        int getDegree(node v);
    };

    /*** 2) Adjacency Matrix Internal representation ***/
    class GraphAdjM : public Graph<GraphAdjM>{
    private:
        friend Graph<GraphAdjM>;
        //Explicitly initialize member which does not have a default constructor (graphAdjM)
        graphAdjM graph = asa::graphAdjM(0);
    public:
        GraphAdjM();
        /*** specializzazioni ***/
        void ResetEachVertex();
        void forEachVertex(node* current_vertex, std::function<void()> f);
        void forEachNeighbor(node v, node* neighbor, std::function<void()> f);
        int getDegree(node v);
    };

    /***  3) Adjacency List Internal representation ***/
    class GraphAdjL : public Graph<GraphAdjL>{
    private:
        friend Graph<GraphAdjL>;
        //Explicitly initialize member which does not have a default constructor (graphAdjM)
        graphAdjL graph = asa::graphAdjL(0);
    public:
        GraphAdjL();
        void ResetEachVertex();
        void forEachVertex(node* current_vertex, std::function<void()> f);
        void forEachNeighbor(node v, node* neighbor, std::function<void()> f);
        int getDegree(node v);
    };
}


#endif //P2_GRAPH_H
