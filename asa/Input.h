//
// Created by salvo on 27/08/21.
//

#ifndef P2_INPUT_H
#define P2_INPUT_H

#include <string>
#include "Graph.h"
#include <concepts>

namespace asa {
    template <typename T>
    concept isGraph = requires(T t){
        std::derived_from<T, Graph>;
    };

    template <isGraph T>
    class Input {
    public:
        static T readInput(const std::string& filename);

    };


}

using asa::Input;
using namespace std;

template<asa::isGraph T>
T Input<T>::readInput(const std::string& filename) {
    unsigned long V,E;
    fstream fin(filename, ios::in);
    if(!fin.is_open()) {
        cout << "errore apertura file fin" << endl;
    }
    fin >> V >> E;
    if(!fin.good()) {
        cout << "errore lettura" << endl;
    }

    T res(V, E);

    string line;
    stringstream lineStream;
    int neighbour;
    for(int i=0; i<=V; i++){
        getline(fin, line);
        lineStream = stringstream(line);
        while(lineStream >> neighbour)
            res.addEdge(i, neighbour);
        //edges.emplace_back(std::pair<int, int>(i,neighbour));
    }
    fin.close();

    res.prepare();
    return res;
}

#endif //P2_INPUT_H
