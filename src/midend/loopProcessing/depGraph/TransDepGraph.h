
#ifndef TRANSDEP_GRAPH
#define TRANSDEP_GRAPH

#include <DepGraph.h>
#include <TransAnalysis.h>
#include <iostream>

template <class Node> class TransDepAnalImpl;
template <class Node>
class TransDepGraphCreate 
   : public IDGraphCreateTemplate<Node,DepInfoSetEdge>, 
     public TransInfoGraph<DepInfoSet>
{
  GraphTransAnalysis<DepInfoSet> anal;
  TransDepAnalImpl<Node> *analop;

//Boolean TransInfoComputed(const GraphNode *src, const GraphNode *snk);
  int TransInfoComputed(const GraphNode *src, const GraphNode *snk);
  DepInfoSet GetTransInfo( const GraphNode *src, const GraphNode *snk);
  void SetTransInfo( GraphNode *src, GraphNode *snk, DepInfoSet info) ;
 public:
  TransDepGraphCreate(GraphAccessTemplate<Node,DepInfoEdge> *g, 
                       int splitlimit=20, 
                       BaseGraphCreate *bc = 0);
  ~TransDepGraphCreate(); 

  DepInfoSetEdge* GetTransDep( Node *n1, Node *n2);
};
  
#endif

