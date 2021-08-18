/*
#include <iostream>
#include "graph/graph.h"
#include <boost/lambda/lambda.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;
    graph a = graph(10);

    a.addEdge(12, 15);
//    a.addEdge(1,2);
    a.addEdge(2,3);
    a.addEdge(3,4);
    return 0;
}
*/

#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <string>
#include <boost/graph/iteration_macros.hpp>
#include <iostream>
#include <fstream>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace boost;
/*
//esempio online modificato con CSR
class WebPage
        {
        public:
            std::string url;
        };

int main2()
{
    typedef std::pair< int, int > E;
    const char* urls[6] = {
            "http://www.boost.org/libs/graph/doc/index.html",
            "http://www.boost.org/libs/graph/doc/table_of_contents.html",
            "http://www.boost.org/libs/graph/doc/adjacency_list.html",
            "http://www.boost.org/libs/graph/doc/history.html",
            "http://www.boost.org/libs/graph/doc/bundles.html",
            "http://www.boost.org/libs/graph/doc/using_adjacency_list.html",
            };

    E the_edges[] = { E(0, 1), E(0, 2), E(0, 3), E(1, 0), E(1, 3), E(1, 5),
                      E(2, 0), E(2, 5), E(3, 1), E(3, 4), E(4, 1), E(5, 0), E(5, 2) };

    typedef compressed_sparse_row_graph< directedS, WebPage > WebGraph;
    WebGraph g(boost::edges_are_sorted, &the_edges[0],
               &the_edges[0] + sizeof(the_edges) / sizeof(E), 6);

    // Set the URLs of each vertex
    int index = 0;
    BGL_FORALL_VERTICES(v, g, WebGraph)
    g[v].url = urls[index++];

    // Output each of the links
    std::cout << "The web graph:" << std::endl;
    BGL_FORALL_EDGES(e, g, WebGraph)
    std::cout << "  " << g[source(e, g)].url << " -> " << g[target(e, g)].url
    << std::endl;

    // Output the graph in DOT format
    dynamic_properties dp;
    dp.property("label", get(&WebPage::url, g));
    std::ofstream out("web-graph.dot");
    write_graphviz_dp(out, g, dp, std::string(), get(vertex_index, g));
    return 0;
}*/

//mio primo esempio ci CSR
int main(){
    struct VertexProps { int id; uint8_t color; };
    std::vector<VertexProps> vs = {{100,1}, {2,1},{3,1}};
    std::vector<std::pair<int, int>> es{{0, 1}, {0, 2}, {2, 1}};

    typedef compressed_sparse_row_graph< bidirectionalS ,VertexProps> Graph;
    Graph g(boost::edges_are_unsorted_multi_pass, std::begin(es), std::end(es), 3);

    int index = 0;
    BGL_FORALL_VERTICES(v, g, Graph){
        g[v].id = vs[index].id;
        g[v].color = vs[index++].color;
    }
    Graph::vertex_iterator v, vend;

    for (boost::tie(v, vend) = vertices(g); v != vend; ++v) {
        std::cout << "Vertex descriptor #" << *v
        //<< " degree:" << boost::degree(*v, g)
        << " id:"     << g[*v].id
        << " color:"  << static_cast<int>(g[*v].color)
        << "\n";
    }
    return 0;
}