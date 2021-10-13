#include <iostream>
#include "Graph/Graph.h"
#include <ctime>
#include "PrintMenu.h"

enum algoritmi {sequential, jones, largest, smallest};
enum int_rep {csr, adjl, adjm};
enum file_input {rgg_15, rgg_16, v100, v1000};
extern int alg, fin, int_rep;
std::string fin_name;

template <typename T>
void startGraphAlgII(T& myGraph){
    switch(alg){
        case sequential: {
            cout << "Algo: sequential\n";
            myGraph.sequential();
            myGraph.clearGraph();
            break;
        }
        case jones: {
            cout << "Algo: JP_mod\n";
            myGraph.JP_mod();
            myGraph.clearGraph();
            break;
        }
        case largest: {
            cout << "Algo: largest degree first\n";
            myGraph.largest_mod();
            myGraph.clearGraph();
            break;
        }
        case smallest: {
            cout << "Algo: smallest degree first\n";
            myGraph.smallest_mod();
            myGraph.clearGraph();
            break;
        }
        default: break;
    }
}

void startGraphAlg(){
    switch(fin){
        case rgg_15:{
            fin_name = "rgg_n_2_15_s0.txt";
            break;
        }
        case rgg_16:{
            fin_name = "rgg_n_2_16_s0.txt";
            break;
        }
        case v100:{
            fin_name = "v100.gra";
            break;
        }
        case v1000:{
            fin_name = "v1000.gra";
            break;
        }
    }
    cout << "File: " << fin_name << '\n';
    switch(int_rep){
        case csr: {
            asa::GraphCSR myGraph(fin_name);
            cout << "Internal representation: CSR\n";
            myGraph.setConcurentThreadsActive(threads);
            startGraphAlgII<asa::GraphCSR>(myGraph);
            break;
        }
        case adjl: {
            asa::GraphAdjL myGraph(fin_name);
            cout << "Internal representation: adjL\n";
            myGraph.setConcurentThreadsActive(threads);
            startGraphAlgII<asa::GraphAdjL>(myGraph);
            break;
        }
        case adjm: {
            asa::GraphAdjM myGraph(fin_name);
            cout << "Internal representation: adjM\n";
            myGraph.setConcurentThreadsActive(threads);
            startGraphAlgII<asa::GraphAdjM>(myGraph);
            break;
        }
        default: break;
    }
}

int main(int argc, char* argv[]) {
    credits();
    //se non ci sono argomenti stampo il menu, altrimenti eseguo direttamente
    if(argc != 1 && argc != 5){
        cerr << "Usare 0 argomenti oppure 5!";
        exit(-1);
    }
    if(argc == 5){
        /*** <.exe> <fin> <internal repres> <alg> ***/
        fin = atoi(argv[1]);
        int_rep = atoi(argv[2]);
        alg = atoi(argv[3]);
        threads = atoi(argv[4]);
    }
    else {
        bootstrap();
    }
    const clock_t begin_alg_time = clock();
    startGraphAlg();
    const clock_t end_alg_time = clock();
    std::cout << "Time alg " << alg << ": " << float( end_alg_time -begin_alg_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    EXIT_SUCCESS;
}



