//
// Created by antonio_vespa on 09/10/21.
//

#ifndef GRAPHCOLORING_PRINTMENU_H
#define GRAPHCOLORING_PRINTMENU_H
#define CMDTABLE_ROWS 8

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

enum file_input {rgg_15, rgg_16, v100, v1000};
namespace fs = boost::filesystem;
namespace po = boost::program_options;

// Global variables
vector<fs::path> files;
bool continue_loop = true;
int n_trials, int_rep = -1, alg = -1, fin = -1,
        threads = std::thread::hardware_concurrency() - LEAVE_FREE;
std::string fin_name;

enum algoritmi {sequential, jones, largest, smallest, l_mod_salvo, l_mod_anto, jp_mod_old, N_ALGO};
string algo_names[] = {"Sequential", "Jones Plassmann",
                       "Largest Degree First", "Smallest Degree First",
                       "Largest Degree First Modified(salvo)",
                       "Largest Degree First Modified(anto)", "jp_mod_old"};

enum int_rep {csr, adjl, adjm, N_REP};
string rep_names[] = {"CSR", "Adjacency Matrix", "Adjacency List"};


// Functions
template <typename T>
void startGraphAlgII(T& myGraph);
void readPath(string path);
void printMenu();
void credits();
void start();
void bootstrap();
void listFiles();

/*** Command table ***/
static struct {
    std::string name;
    void (*func)();
} cmdTable[] = {
        /*** menu ***/
        {"?", printMenu},
        {"q", [](){ exit(0); }},
        /*** operations ***/
        {"start", [](){ continue_loop = false; }},
        {"ct", [](){
            int max = std::thread::hardware_concurrency();
            std::cout << "Digit the number of concurrent threads."
                         " (Default: " << max-LEAVE_FREE  << ", Max: " << max << ")";
            std::cin >> threads;
        }},
        {"fin", [](){
            std::cout << "\nChoose one of the following: \n";
            listFiles();
            std::cout << "Digit the number: " ;
            std::cin >> fin;
        }},
        {"ir", [](){
            std::cout << "\nChoose one of the follower: \n";
            for(int i=0; i<N_REP; i++){
                std::cout << " * " << i << " - " << rep_names[i] << '\n';
            }
            std::cout << "Digit the number: " ;
            std::cin >> int_rep;
        }},
        {"a", [](){
            std::cout << "\nChoose one of the follower: \n";
            for(int i=0; i<N_ALGO; i++){
                std::cout << " * " << i << " - " << algo_names[i] << '\n';
            }
            std::cout << "Digit the number: " ;
            cin >> alg;
            while(cin.fail()) {
                cout << "Insert a number!";
                alg = -1;
                cin >> alg;
            }
            if( alg > N_ALGO )
                cout << "Impossble choice.\n";
        }}
        /*** stats ***/
};


#endif //GRAPHCOLORING_PRINTMENU_H
