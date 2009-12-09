// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
#include "rose.h"
#include <TransDepGraph.h>
#include "TransDepGraphImpl.h"
#include <vector>
#include <TransAnalysis.C>

template class TransInfoGraph<DepInfoSet>;
template class std::vector <TransAnalSCCGraphNode<DepInfoSet>::TwinNode>;
template class GraphTransAnalysis<DepInfoSet>;
