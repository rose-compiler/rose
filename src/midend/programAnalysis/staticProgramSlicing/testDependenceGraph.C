#include "DependenceGraph.h"

int main() {

  DependenceGraph<int> g;

  g.addDependence(1, 2);
  g.addDependence(1, 3);
  g.addDependence(1, 6);
  g.addDependence(2, 4);
  g.addDependence(6, 4);

  g.printGraph();

  return 0;
}
