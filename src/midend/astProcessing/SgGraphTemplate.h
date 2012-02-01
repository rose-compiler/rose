#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
//#include <graphProcessing.h>
#include <staticCFG.h>
#include <interproceduralCFG.h>
#include <rose.h>
struct Vertex{
    SgGraphNode* sg;
    CFGNode cfgnd;
};

struct Edge {
    SgDirectedGraphEdge* gedge;
};

typedef boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::bidirectionalS,
        Vertex,
        Edge
> myGraph;

typedef myGraph::vertex_descriptor VertexID;
typedef myGraph::edge_descriptor EdgeID;

//myGraph* instantiateGraph(SgIncidencedDirectedGraph* g, StaticCFG::CFG cfg);
std::pair<std::vector<SgGraphNode*>, std::vector<SgDirectedGraphEdge*> > getAllNodesAndEdges(SgIncidenceDirectedGraph* g, SgGraphNode* start);
std::map<VertexID, SgGraphNode*> getGraphNode;
std::map<SgGraphNode*, VertexID> VSlink;

myGraph* instantiateGraph(SgIncidenceDirectedGraph*& g, StaticCFG::InterproceduralCFG& cfg, SgNode* pstart) {
    //SgNode* prestart = cfg.getEntry();
    //cfg.buildFullCFG();
    CFGNode startN = cfg.getEntry();
    SgGraphNode* start = cfg.toGraphNode(startN);
    ROSE_ASSERT(startN != NULL);
    ROSE_ASSERT(start != NULL);
    myGraph* graph = new myGraph;
    //std::map<SgGraphNode*, VertexID> VSlink;
    std::pair<std::vector<SgGraphNode*>, std::vector<SgDirectedGraphEdge*> > alledsnds = getAllNodesAndEdges(g, start);
    std::vector<SgGraphNode*> nods = alledsnds.first;
    std::vector<SgDirectedGraphEdge*> eds = alledsnds.second;
    std::set<std::pair<VertexID, VertexID> > prs;
    //for (std::vector<vertex_descriptor> i = nods.begin(); i != nods.end(); i++) {
    //    VertexID vID = boost::add_vertex(graph);
    //    graph[vID].cfgnd = cfg->toCFGNode(*i);
    //}
    for (std::vector<SgDirectedGraphEdge*>::iterator j = eds.begin(); j != eds.end(); j++) {
        SgDirectedGraphEdge* u = *j;
        SgGraphNode* u1 = u->get_from();
        SgGraphNode* u2 = u->get_to();
        VertexID v1;
        VertexID v2;
        if (VSlink.find(u1) == VSlink.end()) {
            v1 = boost::add_vertex(*graph);
            getGraphNode[v1] = u1;
            VSlink[u1] = v1;
            (*graph)[v1].sg = u1;
            (*graph)[v1].cfgnd = cfg.toCFGNode(u1);
        }
        else {
            v1 = VSlink[u1];
        }
        if (VSlink.find(u2) != VSlink.end()) {
            v2 = VSlink[u2];
        }
        else {
            v2 = boost::add_vertex(*graph);
            VSlink[u2] = v2;
            (*graph)[v2].sg = u2;
            getGraphNode[v2] = u2;
            (*graph)[v2].cfgnd = cfg.toCFGNode(u2);
        }
        bool ok;
        EdgeID uE;
        std::pair<VertexID, VertexID> pr;
        pr.first = v1;
        pr.second = v2;
        if (prs.find(pr) == prs.end()) {
            prs.insert(pr);
            boost::tie(uE, ok) = boost::add_edge(v1, v2, *graph);
        }
    }
    //std::cout << "prs.size: " << prs.size() << std::endl;
    return graph;
}



myGraph* instantiateGraph(SgIncidenceDirectedGraph*& g, StaticCFG::CFG& cfg) {
    SgGraphNode* start = cfg.getEntry();
    myGraph* graph = new myGraph;
    //std::map<SgGraphNode*, VertexID> VSlink;
    std::pair<std::vector<SgGraphNode*>, std::vector<SgDirectedGraphEdge*> > alledsnds = getAllNodesAndEdges(g, start);
    std::vector<SgGraphNode*> nods = alledsnds.first;
    std::vector<SgDirectedGraphEdge*> eds = alledsnds.second;
    std::set<std::pair<VertexID, VertexID> > prs;
    //for (std::vector<vertex_descriptor> i = nods.begin(); i != nods.end(); i++) {
    //    VertexID vID = boost::add_vertex(graph);
    //    graph[vID].cfgnd = cfg->toCFGNode(*i);
    //}
    for (std::vector<SgDirectedGraphEdge*>::iterator j = eds.begin(); j != eds.end(); j++) {
        SgDirectedGraphEdge* u = *j;
        SgGraphNode* u1 = u->get_from();
        SgGraphNode* u2 = u->get_to();
        VertexID v1;
        VertexID v2;
        if (VSlink.find(u1) == VSlink.end()) {
            v1 = boost::add_vertex(*graph);
            getGraphNode[v1] = u1;
            VSlink[u1] = v1;
            (*graph)[v1].sg = u1;
            (*graph)[v1].cfgnd = cfg.toCFGNode(u1);
        }
        else {
            v1 = VSlink[u1];
        }
        if (VSlink.find(u2) != VSlink.end()) {
            v2 = VSlink[u2];
        }
        else {
            v2 = boost::add_vertex(*graph);
            VSlink[u2] = v2;
            (*graph)[v2].sg = u2;
            getGraphNode[v2] = u2;
            (*graph)[v2].cfgnd = cfg.toCFGNode(u2);
        }
        bool ok;
        EdgeID uE;
        std::pair<VertexID, VertexID> pr;
        pr.first = v1;
        pr.second = v2;
        if (prs.find(pr) == prs.end()) {
            prs.insert(pr);
            boost::tie(uE, ok) = boost::add_edge(v1, v2, *graph);
        }
    }
    //std::cout << "prs.size: " << prs.size() << std::endl;
    return graph;
}


    
            
std::pair<std::vector<SgGraphNode*>, std::vector<SgDirectedGraphEdge*> > getAllNodesAndEdges(SgIncidenceDirectedGraph* g, SgGraphNode* start) {
    //for (int i = 0; i < starts.size(); i++) {
    SgGraphNode* n = start;
    std::vector<SgGraphNode*> nods;
    std::vector<SgGraphNode*> newnods;
    std::set<SgDirectedGraphEdge*> edsnew;
    std::vector<SgDirectedGraphEdge*> eds;
    std::vector<SgDirectedGraphEdge*> feds;
    std::vector<SgGraphNode*> fnods;
    std::set<std::pair<EdgeID, EdgeID> > prs;
    std::set<SgDirectedGraphEdge*> oeds = g->computeEdgeSetOut(start);
    fnods.push_back(start);
    newnods.push_back(n);
    
    while (oeds.size() > 0) {
        for (std::set<SgDirectedGraphEdge*>::iterator j = oeds.begin(); j != oeds.end(); j++) {
            //if (find(eds.begin(), eds.end(), *j) == eds.end()) {
                if (find(feds.begin(), feds.end(), *j) == feds.end()) {
                    feds.push_back(*j);
                    edsnew.insert(*j);
                }
                if (find(fnods.begin(), fnods.end(), (*j)->get_to()) == fnods.end()) {
                    fnods.push_back((*j)->get_to());
                }
                newnods.push_back((*j)->get_to());
            //}
        }
        
        for (unsigned int i = 0; i < newnods.size(); i++) {
           std::set<SgDirectedGraphEdge*> oedsp = g->computeEdgeSetOut(newnods[i]);
           for (std::set<SgDirectedGraphEdge*>::iterator j = oedsp.begin(); j != oedsp.end(); j++) {
               if (find(feds.begin(), feds.end(), *j) == feds.end()) {
              //    feds.push_back(*j);
                  edsnew.insert(*j);
                  
               }
               //if (find(fnods.begin(), fnods.end(), (*j)->get_to()) == fnods.end()) {
               //    fnods.push_back((*j)->get_to());
               //    newnods.push_back((*j)->get_to());
              // }
    //           newnods.push_back((*j)->get_to());
           }
        }
        nods = newnods;
        oeds = edsnew;
        edsnew.clear();
        newnods.clear();
    }
    std::pair<std::vector<SgGraphNode*>, std::vector<SgDirectedGraphEdge*> > retpr;
    retpr.first = fnods;
    retpr.second = feds;
    //std::cout << "fnods.size()" << fnods.size() << std::endl;
    //std::cout << "feds.size()" << feds.size() << std::endl;
    return retpr;
}
         
