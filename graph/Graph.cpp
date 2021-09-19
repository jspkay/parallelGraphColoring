#include "Graph.h"
#include "fstream"
#include "iostream"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <boost/spirit/include/qi.hpp>
#include <filesystem>
#include <shared_mutex>

using namespace  std;
/* INEFFICIENTE
void Graph::readInput() {
    string file_path = "../graph/benchmark/rgg_n_2_23_s0.txt";
    string line;
    string buffer;
    stringstream lineStream;
    fstream fin(file_path, ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }

    auto file_size = std::filesystem::file_size(file_path);
    buffer.resize(file_size);
    fin.read(buffer.data(),buffer.size());
    istringstream f(buffer);
    getline(f, line);
    lineStream = stringstream(line);
    lineStream >> V >> E;
    if(!f.good()) {
        cout << "errore lettura" << endl;
    }
    this->edges.reserve(E);
    queue<string> q;
    mutex mq;
    condition_variable cv;

    //producer
    auto pro = [&mq, &q, &f, &cv, this](){
        string line;
        for(int i=0; i<V; i++){
            getline(f, line);
            lock_guard<mutex> lk(mq);
            q.emplace(std::to_string(i) + " " + line);
            cv.notify_one();
        }

        lock_guard<mutex> lk(mq);
        q.emplace("stop");
        cv.notify_one();
    };

    //consumer
    auto cons = [&mq, &q, &cv, this](){
        string line;
        stringstream lineStream;
        int neighbour;
        int i;
        while(true){
            unique_lock<mutex> lk(mq);
            cv.wait(lk, [&q](){return !q.empty();});
            line = q.front();
            if(line == "stop"){
                lk.unlock();
                return;
            }
            q.pop();
            lk.unlock();


            lineStream = stringstream(line);
            lineStream >> i;
            while(lineStream >> neighbour)
                edges.emplace_back(std::pair<int, int>(i,neighbour-1));


        }
    };

    //start producer
    thread proT(pro);
    //start consumer
    thread consT(cons);


    fin.close();
    proT.join();
    consT.join();


}*/
void Graph::readInput(string&& fname) {
    auto ext = std::filesystem::path(fname).extension();
    if(ext == ".gra"){
        startingNode = 0;
        Graph::readInputGra(std::move(fname));
    }
    else
        if(ext == ".graph" || ext == ".txt"){
            startingNode = 1;
            readInputGraph(std::move(fname));
        }
        else
            cout << "Wrong input format" << endl;
}

void Graph::readInputGraph(std::string&& fname) {
    //auto file_path = std::filesystem::path("../graph/benchmark/rgg_n_2_15_s0.txt");
    //string file_path = "../graph/benchmark/rgg_n_2_15_s0.txt";
    string line;
    string buffer;
    stringstream lineStream;
    fstream fin(fname, ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }

    //fin.seekg(0, std::ios::end);    //punta all'ultima posizione del file
    //buffer.resize(fin.tellg());     //resize
    //fin.seekg(0);                   //rimette il puntatore all'inizio

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
    //total_set.resize(V); //riservo V posti
    //toColor_set.resize(V/4);
    /***/

    for(int i=0; i<V; i++){
        getline(f, line);
        lineStream = stringstream(line);
        while(lineStream >> neighbour)
            edges.emplace_back(std::pair<int, int>(i,neighbour-1));
    }
    fin.close();
}

void Graph::readInputGra(std::string && fname){
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
    /***  evito riallocazioni dinamiche multiple ***/
    //edges.reserve(E); //riservo E posti,
    //total_set.resize(V); //riservo V posti
    //toColor_set.resize(V/4);
    /***/
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

Graph::Graph() {
    const clock_t begin_time = clock();
    //readInput("../graph/benchmark/rgg_n_2_15_s0.txt");
    readInputGra("../graph/benchmark/v100.gra");
    std::cout << "Time needed to read the graph " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " sec" << std::endl;
    this->graphCSR = GraphCSR(boost::edges_are_unsorted_multi_pass, std::begin(edges), edges.end(), V);
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {
        graphCSR[current_vertex].color = -1;
        graphCSR[current_vertex].random = rand() % 1000 + 1; //1-1000
    }
    concurentThreadsSupported = std::thread::hardware_concurrency();
    cout << "Fine costruzione grafo in formato CSR!\n";
    cout << "******************\n";
    cout << "V:" << V << ", E:" << E;
    cout << "\n******************" << std::endl;
    cout << concurentThreadsSupported << " core available";
}

void Graph::sequential(){
    int C[256], i;
    int8_t color=-1;
    for(int i=0; i<256; i++){
        C[i]=0; //colore non usato
    }
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
        //cout << "current vertex:" << graphCSR[current_vertex].id << ", neigh -> ";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR){
            //cout << graphCSR[neighbor].id << " ";
            if(graphCSR[neighbor].color != -1)
                C[graphCSR[neighbor].color] = 1;
        }
        //cout << "\n";
        //ricerca colore minimo non usato
        for(i=0; i<256; i++){
            if(C[i]==0 && color==-1) //colore non usato
                color=i;
            else
                C[i]=0; //reset colori per il prossimo ciclo
        }
        graphCSR[current_vertex].color = color; //coloro il vertice corrente
        color = -1;
    }
    printOutput();
}

void Graph::printOutput() {
    fstream fout("output.txt", ios::out);
    if(!fout.is_open()) {
        cout << "errore apertura file fout" << endl;
    }

    if(startingNode == 0){
        BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
            fout << "u:" << current_vertex << ", color: " << static_cast<int>(graphCSR[current_vertex].color) << ", rand:" << graphCSR[current_vertex].random << ", degree:" << boost::out_degree(current_vertex, graphCSR) << ", neigh -> ";
            BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
                fout << "(" << neighbor << "," << static_cast<int>(graphCSR[neighbor].color) << ") ";
            }
            fout << "\n";
        }
    }else
    {
        //per rgg
        //ricorda di usare current_vertex+1 dato che gli indici partono da 1 nel file, mentre da 0 nella csr

        BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
            fout << "u:" << current_vertex+1 << ", color: " << static_cast<int>(graphCSR[current_vertex].color) << ", rand:" << graphCSR[current_vertex].random << ", degree:" << boost::out_degree(current_vertex, graphCSR) << ", neigh -> ";
            BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
                fout << "(" << neighbor+1 << "," << static_cast<int>(graphCSR[neighbor].color) << ") ";
            }
            fout << "\n";
        }
    }
}

void Graph::largestDegree(){
    bool major = false;
    int8_t color=-1;
    int C[256], i;
    for(int i=0; i<256; i++){
        C[i]=0; //colore non usato
    }
    BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR) {  //qui ci vuole while
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
            if(boost::out_degree(current_vertex, graphCSR) < boost::out_degree(neighbor, graphCSR))
                break;
            else if(boost::out_degree(current_vertex, graphCSR) == boost::out_degree(neighbor, graphCSR))
                if(graphCSR[current_vertex].random == graphCSR[neighbor].random)
                    break;
                C[graphCSR[neighbor].color] = 1;
                major = true;
        }
        if(major){
            for(i=0; i<256; i++){
                if(C[i]==0 && color==-1) //colore non usato
                    color=i;
                else
                    C[i]=0; //reset colori per il prossimo ciclo
            }
            graphCSR[current_vertex].color = color; //coloro il vertice corrente
            color = -1;
        }
    }
}


