#include <iostream>
#include "Graph/Graph.h"
#include <ctime>
#include "ThreadPool.h"
#define CMDTABLE_ROWS 5

bool continue_loop = true;
std::string fin_name;
int int_rep = -1, alg = -1;

void printMenu(){
    std::cout << "\nGraph Coloring Project menu: \n";
    std::cout << "[fin] Select input file\n";
    std::cout << "[ir] Select internal graph representation\n";
    std::cout << "[a] Select coloring algorithm\n";
    std::cout << "[cc] Compute colors (not implemented yet!)\n";
    std::cout << "[ps] Print stats (not implemented yet!)\n";
    std::cout << "[q] Quit\n" << std::endl;
}

/*** Command table ***/
static struct {
    std::string name;
    void (*func)();
} cmdTable[] = {
        /*** menu ***/
        {"?", printMenu},
        /*** operations ***/
        {"q", [](){ continue_loop = false; }},
        {"fin", [](){
                      std::cout << "\nDigit the name: " ;
                      std::cin >> fin_name;
                      }},
        {"ir", [](){
            std::cout << "\nChoose one of the follower: \n";
            std::cout << " * 1 - CSR\n";
            std::cout << " * 2 - Adjacency Matrix\n";
            std::cout << " * 3 - Adjacency List\n";
            std::cout << "Digit the number: " ;
            std::cin >> int_rep;
        }},
        {"a", [](){
            std::cout << "\nChoose one of the follower: \n";
            std::cout << " * 1 - Jones Plassmann \n";
            std::cout << " * 2 - Largest Degree\n";
            std::cout << " * 3 - Smallest Degree\n";
            std::cout << "Digit the number: " ;
            std::cin >> alg;
        }}
        /*** stats ***/
};

void credits(){
   //std::cout << << std::endl;
   time_t current_time;
   time(&current_time);
   std::cout << "**************************************CREDITS*********************************************\n";
   std::cout << "Graph Coloring Project release 1.3.1, compiled " <<  asctime(localtime(&current_time));
   std::cout << "Fellows of PDS, Politenico di Torino 2020/2021\n";
   std::cout << "Professors:\n";
   std::cout << " * Alessandro Savino\n";
   std::cout << " * Stefano Quer\n";
   std::cout << "Students:\n";
   std::cout << " * Andrea Speziale\n";
   std::cout << " * Salvatore Pappalardo\n";
   std::cout << " * Antonio Vespa\n";
   std::cout << "******************************************************************************************";
}
void start(){
    std::string str;
    std::cout << "\nInitialization Graph Coloring Project...\n";
    int i;
    while(continue_loop){
        std::cout << "\nGraph Coloring Project [? for menu]: ";
        std::cin >> str;
        for(i=0; i<CMDTABLE_ROWS; i++) {
            if(cmdTable[i].name.compare(str)==0) {
                cmdTable[i].func();
                break;
            }
        }
        if(i >= CMDTABLE_ROWS)
            throw std::logic_error("Argument not valid");
    }
}
void bootstrap(){
    credits();
    start();
    return;
}

int main(int argc, char* argv[]) {
    //bootstrap();
    const clock_t begin_time = clock();
    asa::GraphCSR mygraph;
    const clock_t end_construction_time = clock();
    const clock_t begin_seq_time = clock();
    //mygraph.sequential();
    const clock_t end_seq_time = clock();
    mygraph.clearGraph();
    const clock_t begin_jp_time = clock();
    //mygraph.jonesPlassmann();
    const clock_t end_jp_time = clock();
    //mygraph.clearGraph();
    const clock_t begin_ld_time = clock();
    mygraph.largestDegree();
    const clock_t end_ld_time = clock();
    mygraph.clearGraph();
    const clock_t begin_sd_time = clock();
    //mygraph.smallestDegree();
    const clock_t end_sd_time = clock();
    mygraph.clearGraph(); //si fa join dei thread attivati, evita exception
    std::cout << "******************\n";
    std::cout << "Time needed to create the graph without coloring " << float( end_construction_time - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time sequential: " << float( end_seq_time - begin_seq_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time jp: " << float( end_jp_time -begin_jp_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time ld: " << float( end_ld_time -begin_ld_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "Time sd: " << float( end_sd_time -begin_sd_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    EXIT_SUCCESS;
}



