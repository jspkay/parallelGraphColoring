//
// Created by antonio_vespa on 09/10/21.
//

#ifndef GRAPHCOLORING_PRINTMENU_H
#define GRAPHCOLORING_PRINTMENU_H
#define CMDTABLE_ROWS 5

bool continue_loop = true;
int int_rep = -1, alg = -1, fin = -1;

void printMenu(){
    std::cout << "\nGraph Coloring Project menu: \n";
    std::cout << "[fin] Select input file\n";
    std::cout << "[ir] Select internal graph representation\n";
    std::cout << "[a] Select coloring algorithm\n";
    std::cout << "[cc] Compute colors (not implemented yet!)\n";
    std::cout << "[ps] Print stats (not implemented yet!)\n";
    std::cout << "[start] Start\n" << std::endl;
}

/*** Command table ***/
static struct {
    std::string name;
    void (*func)();
} cmdTable[] = {
        /*** menu ***/
        {"?", printMenu},
        /*** operations ***/
        {"start", [](){ continue_loop = false; }},
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
            std::cout << " * 0 - CSR\n";
            std::cout << " * 1 - Adjacency Matrix\n";
            std::cout << " * 2 - Adjacency List\n";
            std::cout << "Digit the number: " ;
            std::cin >> int_rep;
        }},
        {"a", [](){
            std::cout << "\nChoose one of the follower: \n";
            std::cout << " * 0 - Sequential \n";
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
    std::cout << "******************************************************************************************\n";
}

void start(){
    std::string str;
    std::cout << "\nInitialization Graph Coloring Project...\n";
    int i;
    while(continue_loop){
        std::cout << "\nGraph Coloring Project [-? for menu] [-start to start]: ";
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
    start();
    return;
}

#endif //GRAPHCOLORING_PRINTMENU_H
