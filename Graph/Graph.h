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
#define LEAVE_FREE 1


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
        int startingNode;
        int doColor = 0; //semaforo
        unsigned concurrentThreadsActive;

    protected:
        /*** variabili per algoritmi di colorazione ***/
        std::shared_timed_mutex mutex;
        std::condition_variable_any cv;
        std::vector<std::thread> threads;
        std::deque<node> total_set, toColor_set;
        int active_threads;
        int numIteration, increase_numIteration;
        bool isEnded = false;

        void jp_structure(std::function<bool(int,node)>);
        void jp_structure_old(std::function<bool(int,node)>);

    public:
        int searchColor(node u){
            //seleziona il colore minimo e non usato dai nodi vicini al nodo scelto u
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
                if(C[i]==0 && color==-1) //colore i-esimo non usato
                    color=i;
                else
                    C[i]=0; //reset colori per il prossimo ciclo
            }
            return color;
        };
        void clearGraph(){
            //reset strutture dati
            toColor_set.clear();
            total_set.clear();
            //for(std::thread& t : threads)
            //    t.join();
            threads.clear();
            active_threads = concurrentThreadsActive;
            isEnded = false;
            resetEachVertex();
        };
        void fillTotalSet(){
            //riempie deque total_set con i nodi da considerare (ordine id crescente)
            node current_vertex;
            forEachVertex(&current_vertex,[this,&current_vertex](){
                total_set.push_back(current_vertex);
            });
        };
        void resetEachVertex(){
            //resetta i nodi del grafo
            node current_vertex;
            forEachVertex(&current_vertex,[this,&current_vertex](){
                //random escluso
                static_cast<T&>(*this).graph[current_vertex].color = -1;
                static_cast<T&>(*this).graph[current_vertex].num_it = 0;
                static_cast<T&>(*this).graph[current_vertex].toBeDeleted = false;
                static_cast<T&>(*this).graph[current_vertex].weight = -1;
            });
        };
        int computeDegree(node current_vertex){
            //calcola degree di un nodo scelto u (necessario in sd alg)
            int degreeCurrVertex = 0;
            node neighbor;
            forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, &degreeCurrVertex]() {
                if (!static_cast<T &>(*this).graph[neighbor].toBeDeleted)
                    degreeCurrVertex++;  //se non è marcato toBeDeleted, aumento degree
            });
            return degreeCurrVertex;
        }
        void setConcurentThreadsActive(unsigned int cta) {
            Graph::concurrentThreadsActive = cta;
            active_threads = cta;
        }
        /*** IO ***/
        void printOutput(std::string&& name) {
            std::fstream fout(name.c_str(), std::ios::out);
            if (!fout.is_open()) {
                cerr << "errore apertura file fout\n";
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
        void sequential();
        void jonesPlassmann();
        void jonesPlassmann_old();
        void largestDegree_v3();
        void largestDegree_v2();
        void smallest_mod();
        void largestDegree_v1();
        void smallestDegree();
        /*** da specializzare in ogni rappresentazione interna ***/
        void forEachVertex(node* current_vertex, std::function<void()> f){
            return static_cast<T&>(*this).forEachVertex(current_vertex,f);
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
        GraphCSR(vector<std::pair<node, node>>&,int,int);
        /*** specializzazioni ***/
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
        GraphAdjM(vector<std::pair<node, node>>&,int,int);
        /*** specializzazioni ***/
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
        GraphAdjL(vector<std::pair<node, node>>&,int,int);
        /*** specializzazioni ***/
        void forEachVertex(node* current_vertex, std::function<void()> f);
        void forEachNeighbor(node v, node* neighbor, std::function<void()> f);
        int getDegree(node v);
    };
}


#endif //P2_GRAPH_H
