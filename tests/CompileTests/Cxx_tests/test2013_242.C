#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

int main()
{
    using namespace std;
    using namespace boost;

    /* define the graph type
          listS: selects the STL list container to store 
                 the OutEdge list
          vecS: selects the STL vector container to store 
                the vertices
          directedS: selects directed edges
    */
   typedef adjacency_list< listS, vecS, directedS > digraph;

   // instantiate a digraph object with 8 vertices
   digraph g(8);

   // add some edges
   add_edge(0, 1, g);
   add_edge(1, 5, g);
   add_edge(5, 6, g);
   add_edge(2, 3, g);
   add_edge(2, 4, g);
   add_edge(3, 5, g);
   add_edge(4, 5, g);
   add_edge(5, 7, g);

// represent graph in DOT format and send to cout

// Original code is: write_graphviz(cout, g);
// Unparsed as: boost::write_graphviz< class boost::adjacency_list< listS  , vecS  , directedS  , no_property  , no_property  , no_property  , listS  >  > (cout,g);
   write_graphviz(cout, g);

   return 0;
}

