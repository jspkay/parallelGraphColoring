//
// Created by antonio_vespa on 20/10/21.
//

#ifndef GRAPHCOLORING_READINPUT_H
#define GRAPHCOLORING_READINPUT_H

#include <functional>
#include <string>
#include <deque>
#include <forward_list>
#include <vector>
#include <stdexcept>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <shared_mutex>
#include <filesystem>

using namespace std;
typedef unsigned long node;

class ReadInput {
    int V, E;
    std::vector<std::pair<node, node>> edges;
    int startingNode = 0;
public:
    ReadInput(string &fname);

    void readInputGraph(std::string &fname);

    void readInputGra(std::string &fname);

    int getV() const;

    int getE() const;

    const vector<std::pair<node, node>> &getEdges() const;
};


#endif //GRAPHCOLORING_READINPUT_H
