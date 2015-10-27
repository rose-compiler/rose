#ifndef __HH_COLLECTPATHS_HH__
#define __HH_COLLECTPATHS_HH__
#include "rose.h"
#include <vector>
;
class PathCollector
{
private:

    SgIncidenceDirectedGraph* graph;
    StaticCFG::CFG* cfg;
    std::vector<std::vector<SgGraphNode*> > pathSet;
public:

    PathCollector(SgIncidenceDirectedGraph* g,StaticCFG::CFG* t_cfg) {graph = g;cfg=t_cfg;}
//    PathCollector( const std::vector<Link>& links,

    ~PathCollector();
    SgIncidenceDirectedGraph* getGraph() {return graph;}
    void addPath( const std::vector<SgGraphNode*>& p );
    void clearPaths();
    void displayPath(std::vector<SgGraphNode*> p);
    void displayAllPaths();
    std::vector<std::pair<SgNode*,std::string> > getAssociationsForPath(std::vector<SgGraphNode*>);
    //virtual void analyzePath(std::vector<SgGraphNode*> path) = 0;
    std::vector<std::vector<SgGraphNode*> >  getPathSet() { return pathSet; }
    //void traverseAndCollect(SgGraphNode* beginning, SgGraphNode* end) 

};

namespace GraphAlgorithm {

extern SgGraphNode* startNode;
void DepthFirst(PathCollector* pathCollector, std::vector<SgGraphNode*>& visited, SgGraphNode* end, const int& max_hops, const int& min_hops);
bool containsNode(SgGraphNode*,std::vector<SgGraphNode*>);
}
#endif
