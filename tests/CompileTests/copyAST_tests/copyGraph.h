#ifndef COPYGRAPH_H
#define COPYGRAPH_H

//#include "rose.h"
#include <set>
#include <string>

// Run this before copy
std::set<SgNode*> getAllNodes();

// Run this after copy, using the data structure produced before
void graphNodesAfterCopy(const std::set<SgNode*>& nodesBeforeCopy, std::string filename);

#endif // COPYGRAPH_H
