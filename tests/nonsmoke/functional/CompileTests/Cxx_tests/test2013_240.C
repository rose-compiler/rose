#include <iostream>
#include <deque>
#include <iterator>

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

int main()
{
#if 0
 // DQ (6/27/2013): This fails to compile with some versions of boost.

    // Create a n adjacency list, add some vertices.
//  boost::adjacency_list<> g(num tasks);
    boost::adjacency_list<> g(int tasks);
    boost::add_vertex(0, g);
    boost::add_vertex(1, g);
    boost::add_vertex(2, g);
    boost::add_vertex(3, g);
    boost::add_vertex(4, g);
    boost::add_vertex(5, g);
    boost::add_vertex(6, g);

    // Add edges between vertices.
    boost::add_edge(0, 3, g);
    boost::add_edge(1, 3, g);
    boost::add_edge(1, 4, g);
    boost::add_edge(2, 1, g);
    boost::add_edge(3, 5, g);
    boost::add_edge(4, 6, g);
    boost::add_edge(5, 6, g);

    // Perform a topological sort.
    std::deque<int> topo_order;
    boost::topological_sort(g, std::front_inserter(topo_order));

    // Print the results.
    for(std::deque<int>::const_iterator i = topo_order.begin();
        i != topo_order.end();
        ++i)
    {
        cout << tasks[v] << endl;
    }
#endif

    return 0;
}

