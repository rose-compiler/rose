
#include <TransDepGraph.h>
#include "TransDepGraphImpl.h"
#include <vector>
#include <TransAnalysis.C>

template class TransInfoGraph<DepInfoSet>;
template class std::vector <TransAnalSCCGraphNode<DepInfoSet>::TwinNode>;
template class GraphTransAnalysis<DepInfoSet>;
