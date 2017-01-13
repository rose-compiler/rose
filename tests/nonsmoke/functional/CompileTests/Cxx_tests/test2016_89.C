#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"

using namespace boost;

int main(int argc, char *argv[])
{
    //create an -undirected- graph type, using vectors as the underlying containers
    //and an adjacency_list as the basic representation
    typedef adjacency_list<vecS, vecS, undirectedS> UndirectedGraph;

    typedef graph_traits<UndirectedGraph>::edge_iterator edge_iterator;

    return 0;
}
