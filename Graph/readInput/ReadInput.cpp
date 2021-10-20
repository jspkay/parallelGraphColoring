//
// Created by antonio_vespa on 20/10/21.
//

#include "ReadInput.h"

int ReadInput::getV() const {
    return V;
}

int ReadInput::getE() const {
    return E;
}

const vector<std::pair<node, node>> &ReadInput::getEdges() const {
    return edges;
}

ReadInput::ReadInput(string &fname) {
    //redirige alla funzione che legge l'estensione specifica
    auto ext = std::filesystem::path(fname).extension();
    if(ext == ".gra"){
        startingNode = 0;
        readInputGra(fname);
    }
    else
    if(ext == ".graph" || ext == ".txt"){
        startingNode = 1;
        readInputGraph(fname);
    }
    else
        cerr << "Wrong input format" << endl;
}

void ReadInput::readInputGraph(string &fname) {
        //read fileinput .graph
        string line;
        string buffer;
        stringstream lineStream;
        fstream fin(fname, ios::in);
        if(!fin.is_open()) {
            cerr << "errore apertura file fin" << endl;
        }

        auto file_size = std::filesystem::file_size(fname);
        buffer.resize(file_size);
        fin.read(buffer.data(),buffer.size());
        istringstream f(buffer);
        getline(f, line);
        lineStream = stringstream(line);
        lineStream >> V >> E;
        if(!f.good()) {
            cerr << "errore lettura" << endl;
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

void ReadInput::readInputGra(string &fname) {
    //read fileinput .graph
    string line;
    string buffer;
    stringstream lineStream;
    fstream fin(fname, ios::in);
    if(!fin.is_open()) {
        cerr << "errore apertura file fin" << endl;
    }

    auto file_size = std::filesystem::file_size(fname);
    buffer.resize(file_size);
    fin.read(buffer.data(),buffer.size());
    istringstream f(buffer);
    getline(f, line);
    lineStream = stringstream(line);
    lineStream >> V;
    if(!f.good()) {
        cerr << "errore lettura" << endl;
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

/*void readInput(std::string&& fname){
        std::fstream fin(fname, std::ios::in);
        if(!fin.is_open()) {
            std::cerr << "errore apertura file fin" << std::endl;
        }
        fin >> V >> E;
        if(!fin.good()) {
           std::cerr << "errore lettura" << std::endl;
        }
        std::string line;
        std::stringstream lineStream;
        int neighbour;
        // evito riallocazioni dinamiche multiple ... nb: per deque è inutile una funzione reserve
        edges.reserve(E); //riservo E posti,
        for(int i=0; i <= V; i++){
            getline(fin, line);
            lineStream = std::stringstream(line);
            while(lineStream >> neighbour)
                edges.emplace_back(std::pair<int, int>(i,neighbour));
        }
        fin.close();
    };
*/
