#include "interferenceGraph.h"

using namespace graphColoring;
int main()
{
  // randomly generate a graph with 20 nodes, 20% completeness in terms of edges
  Graph g (20, 0.2);
  g.adj_matrix->print();
  g.regenerateAdjacentNeighbors();
  g.toDotGraph("1.dot");

  return 0;
}
