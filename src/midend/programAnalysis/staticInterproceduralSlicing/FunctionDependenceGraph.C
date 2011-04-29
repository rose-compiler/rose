// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 20,5 MB to 10,2MB
#include "sage3basic.h"
#include "DependenceGraph.h"

#include <map>
using namespace std;

FunctionDependenceGraph::FunctionDependenceGraph(ControlDependenceGraph * cdg, DataDependenceGraph * ddg, InterproceduralInfo * ii):
_cdg(cdg), _ddg(ddg)
{
    //_interprocedural = ii;
    mergeGraph(cdg);
    mergeGraph(ddg);
                completeFDG();
    //_summarize();
}
void FunctionDependenceGraph::completeFDG()
{
}


// void FunctionDependenceGraph::_addCDG() {

// set<SimpleDirectedGraphNode *> cdgnodes = _cdg->getNodes();

// //now go through and set up the edges
// set<SimpleDirectedGraphNode *>::iterator i;
// for (i = cdgnodes.begin(); i != cdgnodes.end(); i++) {
// DependenceNode * orig = dynamic_cast<DependenceNode *>(*i);
// DependenceNode * node = _importNode(orig);

// set<SimpleDirectedGraphNode *>::iterator j;
// //go through all of orig's successors and link orig to them
// set<SimpleDirectedGraphNode *> succs = orig->getSuccessors();
// for (j = succs.begin(); j != succs.end(); j++) {
// DependenceNode * orig_succ = dynamic_cast<DependenceNode *>(*j);
// DependenceNode * new_succ = _importNode(orig_succ);
// _establishEdge(node, new_succ, CONTROL);
// //addLink(node, new_succ); //this also sets the predecessors correctly
// }

// }

// }

// void FunctionDependenceGraph::_mergeDDG () {

// set<SimpleDirectedGraphNode *> ddgnodes = _ddg->getNodes();

// set<SimpleDirectedGraphNode *>::iterator i;
// for (i = ddgnodes.begin(); i != ddgnodes.end(); i++) {
// DependenceNode * orig = dynamic_cast<DependenceNode *>(*i);
// DependenceNode * node = _importNode(orig);
// set<SimpleDirectedGraphNode *>::iterator j;
// //go through all of origs successors and link orig to them
// set<SimpleDirectedGraphNode *> succs = orig->getSuccessors();
// for (j = succs.begin(); j != succs.end(); j++) {
// DependenceNode * orig_succ = dynamic_cast<DependenceNode *>(*j);
// DependenceNode * new_succ = _importNode(orig_succ);
// establishEdge(node, new_succ, DATA);
// //addLink(node, new_succ); //this also sets the predecessors correctly
// }
// }
// }

void FunctionDependenceGraph::_summarize()
{/*TODO

    // This is temporary!
    map < SgFunctionCallExp *, InterproceduralInfo::CallSiteStructure >::iterator funcCalls;
    for (funcCalls = _interprocedural->callsite_map.begin();
         funcCalls != _interprocedural->callsite_map.end(); funcCalls++)
    {
        InterproceduralInfo::CallSiteStructure callsite = funcCalls->second;
        map < SgExpression *, DependenceNode * >::iterator i;
        map < SgExpression *, DependenceNode * >::iterator j;
        for (i = callsite.actual_in.begin(); i != callsite.actual_in.end(); i++)
        {
            DependenceNode *dep_from = createNode(i->second);

            // for (j = callsite.actual_out.begin(); j !=
            // callsite.actual_out.end(); j++) {
            // DependenceNode * dep_to = createNode(j->second);
            // establishEdge(dep_from, dep_to, SUMMARY);
            // }
            DependenceNode *dep_to = createNode(callsite.actual_return);

            establishEdge(dep_from, dep_to, SUMMARY);
        }
    }*/
}

set < DependenceNode * >FunctionDependenceGraph::getSlice(DependenceNode * node)
{

    int edgeTypesToFollow = CONTROL | DATA | SUMMARY | CALL/* | RETURN*/;

    set < DependenceNode * >start;
    start.insert(node);

    return _getReachable(start, edgeTypesToFollow);
}
