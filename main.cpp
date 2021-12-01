#include <iostream>
#include "Graph/Graph.h"
#include <ctime>
#include "PrintMenu.h"
#include "Graph/readInput/ReadInput.h"

#include <sched.h>

int ntrial_running_now = 0;

int main(int argc, char *argv[]) {
    //setting linux scheduler
    {
        struct sched_param sp;
        sp.sched_priority = sched_get_priority_max(SCHED_RR);

        int res = sched_setscheduler(0, SCHED_RR, &sp);
        if(res != 0){
            cout << "Please, run the program as root for best performances\n";
        }
    }

    string pathName;
    credits();
    po::variables_map ao; // active options
    po::options_description cmd_options("If the program is executed without "
                                        "an input file, an interactive "
                                        "version will run.\n"
                                        "It is also possibile to run the program "
                                        "without explicit indication, using the "
                                        "following order: input, internal_representation, algorithm, threads[, trials].\n"
                                        "e.g. GraphColoring INPUT_PATH 0 1 2 2, GraphColoring INPUT_PATH 1 1 1 2 5\n"
                                        "Available options");
    cmd_options.add_options()
            ("help,h", "outputs this info message")
            ("input,i", po::value<int>(&fin)->default_value(0), "graph to read")
            ("path,p", po::value<string>(&pathName)->default_value("Graph/benchmark"), "path for the benchmark")
            ("list-files,l", "list files of the path with numbers")
            ("internal-representation,r", po::value<int>(&int_rep)->default_value(0), "internal representation")
            ("algorithm,a", po::value<int>(&alg)->default_value(0), "algorithm to use")
            ("threads,t", po::value<int>(&threads)->default_value(2), "number of threads")
            ("trials,n", po::value<int>(&n_trials)->default_value(1), "number of trials to run");

    po::positional_options_description positionals;
    //positionals.add("path", 1);
    positionals.add("input", 1);
    positionals.add("internal-representation", 1);
    positionals.add("algorithm", 1);
    positionals.add("threads", 1);
    positionals.add("trials", 1);

    try {
        po::store(po::command_line_parser(argc, argv).options(cmd_options).positional(positionals).run(), ao);
        po::notify(ao);
    }
    catch (po::error &e) {
        if (ao.count("help")) {
            cout << cmd_options;
            exit(0);
        }
        cout << e.what() << endl;
        exit(-2);
    }
    readPath(pathName);
    if (ao.count("help")) {
        cout << cmd_options;
        cout << "\nDo you want to start the program in the interactive mode? Press [yes] or [no]\n";
        string in;
        cin >> in;
        if (in.compare("y") == 0 || in.compare("yes") == 0)
            bootstrap();
        else
            return 0;
    }
    if (ao.count("list-files")) {
        cout << "File list:\n";
        listFiles();
        return 0;
    }
    if (!ao.count("input")) {
        bootstrap();
    }
    if (int_rep >= N_REP || int_rep < 0) {
        cout << "ERROR! Bad value: internal_representation. ";
        cout << "Min value: 0; Max value: " << N_REP << endl;
        exit(-1);
    }
    if (alg >= N_ALGO || alg < 0) {
        cout << "ERROR! Bad value: algorithm. ";
        cout << "Min value: 0; Max value: " << N_ALGO << endl;
        exit(-1);
    }
    if (n_trials < 0) {
        cout << "ERROR! Can't run less than 0 trials.";
        exit(-1);
    } else if (n_trials == 0) {
        cout << "Can't run 0 trials. Running just one.";
        n_trials = 1;
    }
    float totalTime = 0;
    int V, E;
    std::vector<std::pair<node, node>> edges;
    fin_name = files[fin].string();
    cout << "File: " << fin_name << '\n';
    if (alg > 0)
        cout << "Num threads: " << threads << '\n';
    ReadInput read(fin_name);
    V = read.getV();
    E = read.getE();
    edges = read.getEdges();
    for (int i = 0; i < n_trials; i++, ntrial_running_now = i) {
        const clock_t begin_alg_time = clock();
        switch (int_rep) {
            case csr: {
                asa::GraphCSR myGraph(edges, V, E);
                if (ntrial_running_now == 0) cout << "Internal representation: CSR\n";
                myGraph.setConcurentThreadsActive(threads);
                startGraphAlgII<asa::GraphCSR>(myGraph);
                break;
            }
            case adjl: {
                asa::GraphAdjL myGraph(edges, V, E);
                if (ntrial_running_now == 0) cout << "Internal representation: adjL\n";
                myGraph.setConcurentThreadsActive(threads);
                startGraphAlgII<asa::GraphAdjL>(myGraph);
                break;
            }
            case adjm: {
                asa::GraphAdjM myGraph(edges, V, E);
                if (ntrial_running_now == 0) cout << "Internal representation: adjM\n";
                myGraph.setConcurentThreadsActive(threads);
                //startGraphAlgII<asa::GraphAdjM>(myGraph);
                break;
            }
            default:
                break;
        }
        const clock_t end_alg_time = clock();
        float t = float(end_alg_time - begin_alg_time) / CLOCKS_PER_SEC;
        std::cout << "  Time trial " << i << ": " << t << " sec\n";
        totalTime += t;
    }
    std::cout << "Average time: " << totalTime / static_cast<float>(n_trials) << "!!!\n";
    EXIT_SUCCESS;
}

template<typename T>
inline void startGraphAlgII(T &myGraph) {
    switch (alg) {
        case sequential: {
            if (ntrial_running_now == 0) cout << "Algorithm: sequential\n";
            myGraph.sequential();
            myGraph.clearGraph();
            break;
        }
        case jones: {
            if (ntrial_running_now == 0) cout << "Algorithm: Jones Plassmann\n";
            myGraph.jonesPlassmann();
            myGraph.clearGraph();
            break;
        }
        case largest_v3: {
            if (ntrial_running_now == 0) cout << "Algorithm: largest degree first v3 - jp structure\n";
            myGraph.largestDegree_v3();
            myGraph.clearGraph();
            break;
        }
        case largest_v1: {
            if (ntrial_running_now == 0) cout << "Algorithm: largest degree first v1 - STL implementation\n";
            myGraph.largestDegree_v1();
            myGraph.clearGraph();
            break;
        }
        case largest_v2: {
            if (ntrial_running_now == 0) cout << "Algorithm: largest degree first v2 - without STL\n";
            myGraph.largestDegree_v2();
            myGraph.clearGraph();
            break;
        }
        case smallest: {
            if (ntrial_running_now == 0) cout << "Algorithm: smallest degree first\n";
            myGraph.smallestDegree();
            myGraph.clearGraph();
            break;
        }
        default:
            break;
    }
}

void listFiles() {
    int i = 0;
    for (auto &el: files) cout << i++ << " - " << el.filename().string() << '\n';
    cout.flush();
}

void readPath(string path) {
    try {
        fs::path p(path);

        if (!fs::exists(p)) {
            cout << "Not valid path!\n";
            exit(-3);
        }

        for (auto &el: fs::directory_iterator(p)) {
            //cout << el << endl;
            if (is_regular_file(el)) files.emplace_back(el);
        }
        std::sort(files.begin(), files.end());
    }
    catch (fs::filesystem_error &ex) {
        cout << ex.what();
        throw ex; // il programma si pianta
    }
}

void credits() {
    //std::cout << << std::endl;
    time_t current_time;
    time(&current_time);
    std::cout << "**************************************CREDITS*********************************************\n";
    std::cout << "Graph Coloring Project release 1.3.1, compiled " << __DATE__ << ' ' << __TIME__
              << '\n';//<<  asctime(localtime(&current_time));
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

void start() {
    std::string str;
    std::cout << "\nInitialization Graph Coloring Project...\n";
    std::cout << "It's possibile to run the program with arguments. Try running the program "
                 "with the -h option.\n";
    int i;
    while (continue_loop) {
        std::cout << "\nGraph Coloring Project [? for menu] [start to start]: ";
        std::cin >> str;
        for (i = 0; i < CMDTABLE_ROWS; i++) {
            if (cmdTable[i].name.compare(str) == 0) {
                cmdTable[i].func();
                break;
            }
        }
        if (i >= CMDTABLE_ROWS)
            cout << "Argument not valid\n";
    }
}

void bootstrap() {
    start();
    return;
}

void printMenu() {
    std::cout << "\nGraph Coloring Project menu: \n"
                 "[fin] Select input file\n"
                 "[ir] Select internal graph representation\n"
                 "[a] Select coloring algorithm\n"
                 "[ct] Select the number of concurrent threads \n"
                 "[nt] Select the number of trials to be executed\n"
                 "[start] Start the algorithm and color the graph\n"
                 "[q] Quit the program" << std::endl;
}
