//
// Created by salvo on 26/08/21.
//

#include "graphColoring.h"

#include <memory>

using asa::graphColoring;

asa::graphColoring::graphColoring(Graph *g) {
    this->g = g;
    colors = std::vector<int>(g->getSize());
    for(int &el : colors) el = -1;
}
asa::graphColoring::~graphColoring() = default;

void graphColoring::setRandoms(int maxValue) {
    randoms = std::make_unique<std::vector<int>>(g->getSize());
    for(auto &el : *randoms.get()){
        rand() % maxValue + 1; // Values between 1 and maxValue
    }
}

std::vector<int> asa::graphColoring::getColors(){
    return colors;
}

void asa::graphColoring::sequentialAlgorithm(){

    Graph &g = *this->g;
    auto &colors = this->colors;
    g.forEachVertex([ &g, &colors](int i){ // for each vertex
        std::set<int> neighColors;

        // find all neighbors color
        g.forEachNeighbor(i, [&neighColors, &colors](int ii){
            if(colors[ii] != -1){
                neighColors.insert(colors[ii]);
            }
        } );

        // and assign the smallest not already assigned to neighbors
        int col = 0;
        for(auto el : neighColors){
            if(el == col) col++;
            else break;
        }
        colors[i] = col;

    } );
}


void graphColoring::largestDegree(){
    bool major = true;
    int C[256]{}, i, ci;
    //deque<GraphCSR::vertex_descriptor> set;

    auto cmp = [this](node a, node b){
        return g->getDegree(a) < g->getDegree(b);
    };
    std::deque<node> verteces;

    g->forEachVertex( [&verteces](node v){
        verteces.push_back(v);
    } );

    /*BGL_FORALL_VERTICES(current_vertex, graphCSR, GraphCSR){
            set.push_back(current_vertex);
        } */

    node current_vertex;
    while(verteces.size() != 0){
        current_vertex = verteces.front();
        g->forEachNeighbor(current_vertex, [&](int neighbor){
            if(colors[neighbor] == -1){
                if(g->getDegree(current_vertex) < g->getDegree(neighbor)){
                    major = false;
                    return;
                }
                else if(g->getDegree(current_vertex) == g->getDegree(neighbor)){
                    if(randoms.get()->at(current_vertex) < randoms.get()->at(neighbor) ){
                        major = false;
                        return;
                    }
                }
            }else{
                C[colors[neighbor]] = 1;
            }
            // TODO: modify forEachNeighbor so that it is possible
            //  to break the loop before time.
        //if(!major) break;
        });
        if(major){
            if(g->getDegree(current_vertex) > 0){
                int color = -1;
                for(int i=0; i<256; i++) {
                    if(C[i] == 0 && color == -1) color = i;
                    else C[i] = 0;
                }
                colors[current_vertex] = color;
            }
            else colors[current_vertex] = 0;
            verteces.pop_front();
        }
        else{
            verteces.pop_front();
            int last = verteces.back();
            verteces.push_back(current_vertex);

            major = true;
            for(int i=0; i<256; i++){
                C[i]=0; //colore non usato
            }
        }

    }
 /*
    GraphCSR::vertex_descriptor current_vertex;
    while(set.size()!=0){
        current_vertex = set.front();
        //cout << current_vertex << ": " << "\n";
        BGL_FORALL_ADJ(current_vertex, neighbor, graphCSR, GraphCSR) {
                //cout << "(" << boost::out_degree(current_vertex, graphCSR) << "," << graphCSR[current_vertex].random << ") vs (" << boost::out_degree(neighbor, graphCSR) << "," << graphCSR[neighbor].random << ")\n";
                if(graphCSR[neighbor].color == -1) { //se non colorato, confronto con il nodo corrente
                    if (boost::out_degree(current_vertex, graphCSR) < boost::out_degree(neighbor, graphCSR)) {
                        major = false;
                        break;
                    } else if (boost::out_degree(current_vertex, graphCSR) == boost::out_degree(neighbor, graphCSR))
                        if (graphCSR[current_vertex].random < graphCSR[neighbor].random) {
                            major = false;
                            break;
                        }
                }
                else //altrimento aggiungo il colore a quelli "già usati"
                    C[graphCSR[neighbor].color] = 1;
            }
        if(major){
            if (boost::out_degree(current_vertex, graphCSR) > 0) {
                int8_t color = -1;
                for (i = 0; i < 256; i++) {
                    if (C[i] == 0 && color == -1) //colore non usato
                        color = i;
                    else
                        C[i] = 0; //reset colori per il prossimo ciclo
                }
                graphCSR[current_vertex].color = color; //coloro il vertice corrente
            }
            else //se non ha vicini, gli do direttamente 0
                graphCSR[current_vertex].color = 0;

            set.pop_front();
        }
        else {
            //se vertice non è stato colorato e lo deve essere, lo reinserisco
            set.pop_front();
            GraphCSR::vertex_descriptor last = set.back();
            set.push_back(current_vertex);
            //reset strutture dati prossimo ciclo
            major = true;
            for(int i=0; i<256; i++){
                C[i]=0; //colore non usato
            }
        }
    } */
}