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
#include <filesystem>

using namespace std;
namespace asa {
    struct vertexDescriptor { int random,num_it,weight; bool toBeDeleted; int16_t color; };
    typedef unsigned long node;
    typedef boost::compressed_sparse_row_graph<boost::bidirectionalS,vertexDescriptor> graphCSR;
    typedef boost::adjacency_matrix<boost::undirectedS,vertexDescriptor> graphAdjM;
    typedef boost::adjacency_list<boost::listS,boost::vecS,boost::directedS,vertexDescriptor> graphAdjL;

    /*** Base class for CRTP ***/
    template <typename T>
    class Graph {
    private:
        friend T;
        Graph(){};
    protected:
        unsigned long V = 0;
        unsigned long E = 0;
        int startingNode = 0;
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
        void readInput(string&& fname) {
            auto ext = std::filesystem::path(fname).extension();
            if(ext == ".gra"){
                startingNode = 0;
                readInputGra(std::move(fname));
            }
            else
                if(ext == ".graph" || ext == ".txt"){
                    startingNode = 1;
                    readInputGraph(std::move(fname));
                }
                else
                    cout << "Wrong input format" << endl;
        }
        void readInputGraph(std::string&& fname) {
            string line;
            string buffer;
            stringstream lineStream;
            fstream fin(fname, ios::in);
            if(!fin.is_open()) {
                cout << "errore apertura file fin" << endl;
            }

            auto file_size = std::filesystem::file_size(fname);
            buffer.resize(file_size);
            fin.read(buffer.data(),buffer.size());
            istringstream f(buffer);
            getline(f, line);
            lineStream = stringstream(line);
            lineStream >> V >> E;
            if(!f.good()) {
                cout << "errore lettura" << endl;
            }

            int neighbour;
            /***  evito riallocazioni dinamiche multiple ***/
            edges.reserve(E); //riservo E posti,
            /***/

            for(int i=0; i<V; i++){
                getline(f, line);
                lineStream = stringstream(line);
                while(lineStream >> neighbour)
                    edges.emplace_back(std::pair<int, int>(i,neighbour-1));
            }
            fin.close();
        }

        void readInputGra(std::string && fname){
            string line;
            string buffer;
            stringstream lineStream;
            fstream fin(fname, ios::in);
            if(!fin.is_open()) {
                cout << "errore apertura file fin" << endl;
            }

            auto file_size = std::filesystem::file_size(fname);
            buffer.resize(file_size);
            fin.read(buffer.data(),buffer.size());
            istringstream f(buffer);
            getline(f, line);
            lineStream = stringstream(line);
            lineStream >> V;
            if(!f.good()) {
                cout << "errore lettura" << endl;
            }

            int neighbour;
            string delimiter = ": ";
            size_t pos = 0;
            int j;
            for(int i=0; i<V; i++){
                getline(f, line);
                lineStream = stringstream(line);
                lineStream >> j;
                pos = line.find(delimiter);
                lineStream = stringstream(line.substr(pos+delimiter.length()));

                while(lineStream >> neighbour){
                    edges.emplace_back(std::pair<int, int>(j,neighbour));
                    edges.emplace_back(std::pair<int, int>(neighbour,j));
                    E += 2;
                }
            }
            fin.close();
        }
//        void readInput(std::string&& fname){
//            std::fstream fin(fname, std::ios::in);
//            if(!fin.is_open()) {
//                std::cerr << "errore apertura file fin" << std::endl;
//            }
//            fin >> V >> E;
//            if(!fin.good()) {
//                std::cerr << "errore lettura" << std::endl;
//            }
//            std::string line;
//            std::stringstream lineStream;
//            int neighbour;
//            /***  evito riallocazioni dinamiche multiple ... nb: per deque è inutile una funzione reserve ***/
//            edges.reserve(E); //riservo E posti,
//            /***/
//            for(int i=0; i <= V; i++){
//                getline(fin, line);
//                lineStream = std::stringstream(line);
//                while(lineStream >> neighbour)
//                    edges.emplace_back(std::pair<int, int>(i,neighbour));
//            }
//            fin.close();
//        };
        int searchColor(node u){
            int i,lastColorFound=0;
            int C[2048]={};
            int16_t color=-1;
            node neighbor;
            node current_vertex = u;
            forEachNeighbor(current_vertex,&neighbor,[this,&current_vertex,&neighbor,&C, &lastColorFound](){
                if(static_cast<T&>(*this).graph[neighbor].color != -1) {
                    C[static_cast<T &>(*this).graph[neighbor].color] = 1; //segno il colore del vicino
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
            return color;
        };
        void clearGraph(){
            toColor_set.clear();
            total_set.clear();
            for(std::thread& t : threads)
                t.join();
            threads.clear();
            active_threads = concurentThreadsAvailable;
            isEnded = false;
            ResetEachVertex();
        };
        void printOutput(std::string&& name) {
            std::fstream fout(name.c_str(), std::ios::out);
            if (!fout.is_open()) {
                std::cerr << "errore apertura file fout\n";
            }
            node current_vertex;
            if(startingNode == 0){
                forEachVertex(&current_vertex,[this,&current_vertex,&fout](){
                    fout << "u:" << current_vertex << ", color: " <<
                    static_cast<int>(static_cast<T&>(*this).graph[current_vertex].color) << ", rand:" << static_cast<T&>(*this).graph[current_vertex].random << ", degree:" <<
                    boost::out_degree(current_vertex, static_cast<T&>(*this).graph) << ", weight: " << static_cast<T&>(*this).graph[current_vertex].weight << ", neigh -> ";
                    node neighbor;
                    forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &fout]() {
                        fout << "(" << neighbor << "," << static_cast<int>(static_cast<T&>(*this).graph[neighbor].color) << ") ";
                    });
                    fout << "\n";

                });
            }
            else
            {
                //per rgg
                //ricorda di usare current_vertex+1 dato che gli indici partono da 1 nel file, mentre da 0 nella csr
                forEachVertex(&current_vertex,[this,&current_vertex,&fout](){
                    fout << "u:" << current_vertex+1 << ", color: " <<
                    static_cast<int>(static_cast<T&>(*this).graph[current_vertex].color) << ", rand:" << static_cast<T&>(*this).graph[current_vertex].random << ", degree:" <<
                    boost::out_degree(current_vertex, static_cast<T&>(*this).graph) << ", weight: " << static_cast<T&>(*this).graph[current_vertex].weight << ", neigh -> ";
                    node neighbor;
                    forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &fout]() {
                        fout << "(" << neighbor+1 << "," << static_cast<int>(static_cast<T&>(*this).graph[neighbor].color) << ") ";
                    });
                    fout << "\n";

                });
            }
            fout.close();
        };
        /*** algoritmi colorazione ***/
        void sequential(){
            int16_t color=-1;
            node current_vertex;
            forEachVertex(&current_vertex,[this,&color,&current_vertex](){
                color = searchColor(current_vertex);
                static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                color = -1;
            });
            printOutput("sequential-output.txt");
        };
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
                        /*** terminazione thread ***/
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
                        if(major) {
                            std::shared_lock<std::shared_timed_mutex> slk(mutex);
                            /*** colorazione ***/
                            int16_t color = -1;
                            color = searchColor(current_vertex);
                            slk.unlock();
                            ulk.lock();
                            static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                        }
                        else {
                            ulk.lock();
                            /*** reinserisco (nodo da colorare successivamente) ***/
                            total_set.push_back(current_vertex);
                            major=true;
                        }
                        cv.notify_all();
                    }
                });
            }
            /////////////////////////////////////////////////////////////////////////////
            /*** main thread ***/
            std::unique_lock<std::shared_timed_mutex> ulk(mutex);
            //cout << "--------->ottenuto unique" << endl;
            cv.wait(ulk, [this]() { return isEnded == true; });
            printOutput("largestDegree-output.txt");
        };
        void jonesPlassmann(){
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
                        //cout << static_cast<T&>(*this).graph[current_vertex].num_it << "-" << numIteration << " _____ thread: " << this_thread::get_id() << " aspetto!" << endl;
                        cv.wait(ulk,[this,current_vertex](){ return static_cast<T&>(*this).graph[current_vertex].num_it <= numIteration || !increase_numIteration; });
                        //cout << "thread: " << this_thread::get_id() << " riavviato!" << endl;
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
                    //cout << "thread: " << this_thread::get_id() << " terminato!" << endl;
                });
            }
            /////////////////////////////////////////////////////////////////////////////
            /*** main thread ***/
            while(true) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                //cout << "--------->ottenuto unique" << endl;
                cv.wait(ulk, [this]() { return isEnded == true || increase_numIteration == active_threads - 1; });  //NON concurrent !!!!
                if(isEnded)
                    break;
                //means -> increase_numIteration == true
                while(toColor_set.size()!=0){
                    //coloro tutti con stesso colore
                    current_vertex = toColor_set.front();
                    toColor_set.pop_front();
                    int16_t color = -1;
                    color = searchColor(current_vertex);
                    static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                    //if(graphCSR[current_vertex].color > 10)
                    //cout << "jp: " << static_cast<int>(graphCSR[current_vertex].color) << endl;
                }
                increase_numIteration = 0;
                numIteration ++; //posso iniziare un nuovo giro
                //cout << "rimanenti: " << total_set.size() << endl;
                cv.notify_all();
                //cout << ">---------fine unique" << endl;
            }
            //////////////////////////////////////////////////////////////////////////
            printOutput("jp-output.txt");
        };
        void smallestDegree(){
            /***
                 weighting phase
                 k = numIteration - i = current_color
             ***/
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
                        /*** terminazione thread ***/
                        if(total_set.size()==0){
                            active_threads--;
                            if(active_threads==1){
                                isEnded = true;
                                cv.notify_one();
                            }
                            break;
                        }
                        /*** pop dalla coda ***/
                        current_vertex = total_set.front();
                        total_set.pop_front();
                        //SE STO INIZIANDO UN NUOVO GIRO, ALLORA ASPETTO CHE TUTTI PRIMA SIANO PESATI
                        if(static_cast<T&>(*this).graph[current_vertex].num_it > numIteration){
                            increase_numIteration++;
                            cv.notify_all();
                        }
                        cv.wait(ulk,[this,current_vertex](){ return static_cast<T&>(*this).graph[current_vertex].num_it <= numIteration || !increase_numIteration; });
                        ulk.unlock();
                        //////////////////////////////////////////////////////////////////////
                        std::shared_lock<std::shared_timed_mutex> slk(mutex);
                        /*** calcolo degree current_vertex (non posso usare getDegree) ***/
                        int degreeCurrVertex = 0;
                        node neighbor;
                        forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &degreeCurrVertex](){
                            if(!static_cast<T&>(*this).graph[neighbor].toBeDeleted)
                                degreeCurrVertex++;  //se non è marcato toBeDeleted, aumento degree
                        });
                        /*** current_vertex ha smallest degree? ***/
                        if (degreeCurrVertex <= numIteration){
                            //std::cout << degreeCurrVertex << std::endl;
                            minor = true;
                        }
                        slk.unlock();
                        //////////////////////////////////////////////////////////////////////
                        ulk.lock();
                        //aggiungo a vertici da pesare se maggiore
                        if(minor){
                            /*** peso ***/
                            toColor_set.push_back(current_vertex);
                        }
                        else {
                            /*** reinserisco in coda,valuto al prossimo giro ***/
                            static_cast<T&>(*this).graph[current_vertex].num_it++;
                            total_set.push_back(current_vertex);
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
                cv.wait(ulk, [this]() { return isEnded == true || increase_numIteration == active_threads - 1; });
                if(isEnded)
                    break;
                //arrivato qui means -> increase_numIteration == concurentThreadsAvailable - 1
                bool doIHaveToIncreaseColor = toColor_set.size()!=0;
                while(toColor_set.size()!=0){
                    /*** pop da coda + weight + mark deleted ***/
                    current_vertex = toColor_set.front();
                    toColor_set.pop_front();
                    static_cast<T&>(*this).graph[current_vertex].weight = current_weigth; //peso il vertice corrente
                    static_cast<T&>(*this).graph[current_vertex].toBeDeleted = true;
                }
                if(doIHaveToIncreaseColor)  //se almeno un nodo aveva il current_weigth, allora devo incrementarlo per il prossimo giro
                    current_weigth++;
                increase_numIteration = 0;
                numIteration ++;
                cv.notify_all();
                //std::cout << "rimanenti: " << total_set.size() << std::endl;
                //cout << ">---------fine unique" << endl;
            }
            total_set.clear();
            //////////////////////////////////////////////////////////////////////////
            /***
                 coloring phase
             ***/
            forEachVertex(&current_vertex,[this,&current_vertex](){
                total_set.push_back(current_vertex);
            });
            int C[256]{},wei;
            for(wei = current_weigth; wei>=0; wei--){
                forEachVertex(&current_vertex,[this,&current_vertex,current_weigth,wei,&C](){
                    current_vertex = total_set.front();
                    total_set.pop_front();
                    if(static_cast<T&>(*this).graph[current_vertex].weight == wei) {
                        /*** coloring ***/
                        int16_t color = -1;
                        color = searchColor(current_vertex);
                        static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                    }
                    else
                        /*** reinserisco se weight minore ***/
                        total_set.push_back(current_vertex);
                    //std::cout << "rimanenti: " << total_set.size() << std::endl;
                });
            };
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
