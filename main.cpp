#include <iostream>
#include "Graph/Graph.h"
#include <ctime>
#include "PrintMenu.h"
#include "boost/program_options.hpp"

extern int alg, fin, int_rep;
std::string fin_name;

template <typename T>
inline void startGraphAlgII(T& myGraph){
    switch(alg){
        case sequential: {
            cout << "Algo: sequential\n";
            myGraph.sequential();
            myGraph.clearGraph();
            break;
        }
        case jones: {
            cout << "Algo: JP_mod\n";
            myGraph.jonesPlassmann();
            myGraph.clearGraph();
            break;
        }
        case largest: {
            cout << "Algo: largest degree first\n";
            myGraph.ldf();
            myGraph.clearGraph();
            break;
        }
        case l_mod_anto:{
            cout << "Algo: largest degree first modified_anto\n";
            myGraph.largestDegree();
            myGraph.clearGraph();
            break;
        }
        case l_mod_salvo:{
            cout << "Algo: largest degree first modified_salvo\n";
            myGraph.ldf_mod();
            myGraph.clearGraph();
            break;
        }
        case smallest: {
            cout << "Algo: smallest degree first\n";
            myGraph.smallestDegree();
            myGraph.clearGraph();
            break;
        }
        default: break;
    }
}

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    credits();
    po::options_description cmd_options("If the program is executed without "
                                        "an input file, an interactive "
                                        "version will run.\n"
                                        "It is also possibile to run the program "
                                        "without explicit indication, using the "
                                        "following order: input, internal_representation, algorithm, threads[, trials].\n"
                                        "e.g. GraphColoring 0 1 2 2; GraphColoring 1 1 1 2 5\n"
                                        "Available options");
    cmd_options.add_options()
            ("help,h", "outputs this info message")
            ("input,i", po::value<int>(&fin), "graph to read")
            ("internal-representation,r", po::value<int>(&int_rep)->default_value(0), "internal representation")
            ("algorithm,a", po::value<int>(&alg)->default_value(0), "algorithm to use")
            ("threads,t", po::value<int>(&threads)->default_value(2), "number of threads")
            ("trials,n", po::value<int>(&n_trials)->default_value(1),"number of trials to run")
    ;

    po::positional_options_description positionals;
    positionals.add("input", 1);
    positionals.add("internal-representation", 1);
    positionals.add("algorithm", 1);
    positionals.add("threads", 1);
    positionals.add("trials", 1);

    po::variables_map ao; // active options
    try{
        po::store(po::command_line_parser(argc, argv).options(cmd_options).positional(positionals).run(), ao);
    }catch(std::exception e){
        cout << "ERROR! Parameter not recognized\n";
        exit(-2);
    }
    po::notify(ao);

    if(ao.count("help")){
        cout << cmd_options << endl;
        return 0;
    }
    if(!ao.count("input")) {
        bootstrap();
    }
    if(int_rep >= N_REP || int_rep < 0) {
        cout << "ERROR! Bad value: internal_representation. ";
        cout << "Min value: 0; Max value: " << N_REP << endl;
        exit(-1);
    }
    if(alg >= N_ALGO || alg < 0) {
        cout << "ERROR! Bad value: algorithm. ";
        cout << "Min value: 0; Max value: " << N_ALGO << endl;
        exit(-1);
    }
    if(n_trials < 0) {
        cout << "ERROR! Can't run less than 0 trials.";
        exit(-1);
    }
    else if(n_trials == 0){
        cout << "Can't run 0 trials. Running just one.";
        n_trials = 1;
    }

    float totalTime = 0;
    for(int i=0; i<n_trials; i++) {
        const clock_t begin_alg_time = clock();
        switch (fin) {
            case rgg_15: {
                fin_name = "rgg_n_2_15_s0.txt";
                break;
            }
            case rgg_16: {
                fin_name = "rgg_n_2_16_s0.txt";
                break;
            }
            case v100: {
                fin_name = "v100.gra";
                break;
            }
            case v1000: {
                fin_name = "v1000.gra";
                break;
            }
        }
        cout << "File: " << fin_name << '\n';
        cout << int_rep;
        switch (int_rep) {
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
            default:
                break;
        }
        const clock_t end_alg_time = clock();
        float t = float(end_alg_time - begin_alg_time) / CLOCKS_PER_SEC;
        std::cout << "Time trial " << i << ": " << t << " sec" << std::endl;
        totalTime += t;
    }
    std::cout << "Average time: " << totalTime/static_cast<float>(n_trials) << std::endl;
    EXIT_SUCCESS;
}



