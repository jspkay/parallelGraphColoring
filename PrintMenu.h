//
// Created by antonio_vespa on 09/10/21.
//

#ifndef GRAPHCOLORING_PRINTMENU_H
#define GRAPHCOLORING_PRINTMENU_H
#define CMDTABLE_ROWS 8

enum algoritmi {sequential, jones, largest, smallest, l_mod_salvo, l_mod_anto, N_ALGO};
string algo_names[] = {"Sequential", "Jones Plassmann",
                       "Largest Degree First", "Smallest Degree First",
                       "Largest Degree First Modified(salvo)",
                       "Largest Degree First Modified(anto)"};

enum int_rep {csr, adjl, adjm, N_REP};
string rep_names[] = {"CSR", "Adjacency Matrix", "Adjacency List"};

enum file_input {rgg_15, rgg_16, v100, v1000};

bool continue_loop = true;
int n_trials, int_rep = -1, alg = -1, fin = -1,
        threads = std::thread::hardware_concurrency() - LEAVE_FREE;

void printMenu(){
    std::cout << "\nGraph Coloring Project menu: \n"
        "[fin] Select input file\n"
        "[ir] Select internal graph representation\n"
        "[a] Select coloring algorithm\n"
        "[cc] Compute colors (not implemented yet!)\n"
        "[ps] Print stats (not implemented yet!)\n"
        "[ct] Select the number of concurrent threads \n"
        "[nt] Select the number of trials to be executed\n"
        "[start] Start the algorithm and color the graph\n"
        "[q] Quit the program" << std::endl;
}

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
            std::cout << "\nChoose one of the follower: \n";
            std::cout << " * 0 - rgg_15\n";
            std::cout << " * 1 - rgg_16\n";
            std::cout << " * 2 - v100\n";
            std::cout << " * 3 - v1000\n";
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
                cin >> alg;
            }
            if( alg > N_ALGO )
                cout << "Impossble choice.\n";
        }}
        /*** stats ***/
};

void credits(){
    //std::cout << << std::endl;
    time_t current_time;
    time(&current_time);
    std::cout << "**************************************CREDITS*********************************************\n";
    std::cout << "Graph Coloring Project release 1.3.1, compiled " << __DATE__ << ' ' << __TIME__ << '\n';//<<  asctime(localtime(&current_time));
    std::cout << "Fellows of PDS, Politenico di Torino 2020/2021\n";
    std::cout << "Professors:\n";
    std::cout << " * Alessandro Savino\n";
    std::cout << " * Stefano Quer\n";
    std::cout << "Students:\n";
    std::cout << " * Andrea Speziale\n";
    std::cout << " * Salvatore Pappalardo\n";
    std::cout << " * Antonio Vespa\n";
    std::cout << "******************************************************************************************\n";
}

void start(){
    std::string str;
    std::cout << "\nInitialization Graph Coloring Project...\n";
    std::cout << "It's possibile to run the program with arguments. Try running the program "
                 "with the -h option.\n";
    int i;
    while(continue_loop){
        std::cout << "\nGraph Coloring Project [? for menu] [start to start]: ";
        std::cin >> str;
        for(i=0; i<CMDTABLE_ROWS; i++) {
            if(cmdTable[i].name.compare(str)==0) {
                cmdTable[i].func();
                break;
            }
        }
        if(i >= CMDTABLE_ROWS)
            cout << "Argument not valid\n";
    }
}

void bootstrap(){
    start();
    return;
}

#endif //GRAPHCOLORING_PRINTMENU_H
