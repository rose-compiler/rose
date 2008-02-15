#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rose.h"

#include "SummaryGraph.h"

TDPGraph::TDPGraph(AttributeEquations ae) {

  //go through each individual equation. For each attribute that
  //appears on the rhs of the equation, insert an edge from it to the
  //attribute on the lhs of the equation. If the nodes don't already
  //exist in the graph, they will be created.

  AttributeEquations::iterator i;

  for (i = ae.begin(); i != ae.end(); i++) {
    DependenceNode * to = createNode(i->first);
    set<DependenceNode *>::iterator j;
    for (j = i->second.begin(); j != i->second.end(); j++) {
      DependenceNode * from = createNode(*j);
      establishEdge(from, to, UNMARKED);
    }
  }
}

set<DependenceGraph::Edge> TDPGraph::AddEdgeAndClose(DependenceNode * from, DependenceNode * to) {
  set<Edge> retval;
 
  //These are the edges left to process
  stack<Edge> toProcess;
 
  toProcess.push(Edge(from, to));

  //Until process is empty, for each edge, we add it to the graph, and
  //process any edges that are added to produce transitive closure.
  while (!toProcess.empty()) {
    //get the first element
    Edge e = toProcess.top();
    toProcess.pop();

    //if the edge is already in TDP, we continue (all edges in TDP are unmarked)
    if (edgeExists(e.first, e.second, UNMARKED))
      continue;

    //add it to the graph and see if anything else needs to be added
    establishEdge(e.first, e.second, UNMARKED);
    retval.insert(e);
    set<Edge> newEdges = _closure(e);
    for (set<Edge>::iterator i = newEdges.begin(); i != newEdges.end(); i++) {
      toProcess.push(*i);
    }

    //add the roots of the current edge to the return set
    retval.insert(Edge(_getAttributeNode(e.first->getCopiedFrom()), _getAttributeNode(e.second->getCopiedFrom())));
  }

  return retval;
}

set<DependenceGraph::Edge> TDPGraph::_closure(Edge e) {
  //precondition: The graph is transitively closed. We have added an
  //edge, so to transitively close the graph again, all edges starting
  //from "to" induce an edge from "from." Similarly, all edges ending
  //at "from," induce an edge to "to."

  set<Edge> retval;

  DependenceNode * from = e.first;
  DependenceNode * to = e.second;

  //get all the nodes that succeed "to"
  set<DirectedGraphNode *> succs = to->getSuccessors();
  //for each of these nodes, add an edge from "from" to them
  for (set<DirectedGraphNode *>::iterator i = succs.begin(); i != succs.end(); i++) {
    DependenceNode * new_to = dynamic_cast<DependenceNode *>(*i);
    retval.insert(Edge(from, new_to));
  }

  //get all the nodes that precede "from"
  set<DirectedGraphNode *> preds = from->getPredecessors();
  //for each of these nodes, add an edge from them to "to"
  for (set<DirectedGraphNode *>::iterator i = preds.begin(); i != preds.end(); i++) {
    DependenceNode * new_from = dynamic_cast<DependenceNode *>(*i);
    retval.insert(Edge(new_from, to));
  }

  return retval;
}
    
DependenceNode * TDPGraph::_getAttributeNode(DependenceNode * node) {
  //if the node is an ACTUAL node, retrieve the FORMAL node it points to (or is pointed to by it)
  if (node->isActual()) {
    DependenceNode * formal = NULL;
    set<DirectedGraphNode *> preds = node->getPredecessors();
    set<DirectedGraphNode *> succs = node->getSuccessors();
    //go through preds looking for a FORMAL node
    for (set<DirectedGraphNode *>::iterator i = preds.begin(); i != preds.end(); i++) {
      DependenceNode * n = dynamic_cast<DependenceNode *>(*i);
      if (n->isFormal()) {
	formal = n;
	break;
      }
    }
    //if we still haven't found the FORMAL node, go through succs
    if (formal == NULL) {
      for (set<DirectedGraphNode *>::iterator i = succs.begin(); i != succs.end(); i++) {
	DependenceNode * n = dynamic_cast<DependenceNode *>(*i);
	if (n->isFormal()) {
	  formal = n;
	  break;
	}
      }
    }
    node = formal;
    ROSE_ASSERT(node != NULL);
  }
  ROSE_ASSERT(node->isFormal());
  //now get the version from the pdg's _interprocedural object
  return (node->getCopiedFrom());
}
