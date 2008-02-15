#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rose.h"

#include "SummaryGraph.h"

SummaryGraph::SummaryGraph(SystemDependenceGraph * sdg) : _sdg(sdg) {

  //create the attribute definition equations for the SDG
  _ade = new AttributeDefinitionEquations(_sdg);

  //initialize the tds and tdp graphs
  _initializeGraphs();

  //initialize the edges in the tds graphs
  _initializeEdges();
}

void SummaryGraph::_initializeGraphs() {
  //for every PDG in the SDG, create a TDSGraph
  set<ProgramDependenceGraph *> pdgs = _sdg->getPDGs();
  set<ProgramDependenceGraph *>::iterator i;
  for (i = pdgs.begin(); i != pdgs.end(); i++) {
    InterproceduralInfo * ii = (*i)->getInterprocedural();
    TDSGraph * tds = new TDSGraph(ii);
    /*add a link from every attribute in the TDSGraph to the TDSGraph
    this is equivalent to linking all the formal attributes in ii to
    TDSGraph
    */
    //link formal-return
    _tdsgraph_map[ii->procedureEntry.formal_return] = tds;
    //link formal-in
    map<SgInitializedName *, DependenceNode *>::iterator j;
    for (j = ii->procedureEntry.formal_in.begin(); j != ii->procedureEntry.formal_in.end(); j++) {
      _tdsgraph_map[j->second] = tds;
    }
    //link formal-out
    for (j = ii->procedureEntry.formal_out.begin(); j != ii->procedureEntry.formal_out.end(); j++) {
      _tdsgraph_map[j->second] = tds;
    }

    _tdsgraphs.insert(tds);

    //create a TDPGraph for the PDG and add it to our list of TDPGraphs
    AttributeEquations ae = _ade->getEquations(*i);
    TDPGraph * tdp = new TDPGraph(ae);
    _tdpgraphs.insert(tdp);
    //the interproc info ii has its attributes used in tdp. mark this
    _tdpgraph_map[ii].insert(tdp);
  }
}


void SummaryGraph::_initializeEdges() {

  for (set<TDPGraph *>::iterator i = _tdpgraphs.begin(); i != _tdpgraphs.end(); i++) {
    TDPGraph * tdp = *i;

    //For every edge in the TDPGraph, if both endpoints are from the
    //same non-terminal, insert the edge into the appropriate TDPGraph
    set<DirectedGraphNode *> tdpnodes = tdp->getNodes();
    for (set<DirectedGraphNode *>::iterator j = tdpnodes.begin(); j != tdpnodes.end(); j++) {
      //get the DependenceNode
      DependenceNode * tdpnode_from = dynamic_cast<DependenceNode *>(*j);
      //get the original ii node this tdpnode was built from
      DependenceNode * iinode_from = tdpnode_from->getCopiedFrom();
      ROSE_ASSERT(iinode_from != NULL);
      //get the tds graph this node belongs to
      TDSGraph * tds_from = _tdsgraph_map[iinode_from];

      //go through the successors of tdpnode_from and see if any of them are in the same tds graph
      set<DirectedGraphNode *> succs = tdpnode_from->getSuccessors();
      for (set<DirectedGraphNode *>::iterator k = succs.begin(); k != succs.end(); k++) {
	//get the DependenceNode
	DependenceNode * tdpnode_to = dynamic_cast<DependenceNode *>(*k);
	//get the original ii node
	DependenceNode * iinode_to = tdpnode_to->getCopiedFrom();
	ROSE_ASSERT(iinode_to != NULL);
	//get the tds graph this node belongs to
	TDSGraph * tds_to = _tdsgraph_map[iinode_to];

	//if the two graphs are the same, add the unmarked edge
	if (tds_from == tds_to) {
	  tds_to.pushUnmarked(Edge(iinode_to, iinode_from));
	}
      }
    }
  }
}

void SummaryGraph::_setTransitiveDependences() {
  //while there are TDSGraphs with unmarked edges
  for (set<TDSGraph *>::iterator i = _tdsgraphs.begin(); i != _tdsgraphs.end(); i++) {
    TDSGraph * tds = *i;
    //if there are unmarked edges, we go through them and process them
    while (tds->hasUnmarked()) {
      //get an unmarked edge
      Edge e = tds->popUnmarked();
      //if it's already in tds, we can continue
      if (tds->edgeExists(tds->getNode(e.first), tds->getNode(e.second)))
	continue;

      //add it to TDS
      tds->establishEdge(tds->getNode(e.first), tds->getNode(e.second), MARKED);
      
      //get the TDPGraphs that use these attributes
      set<TDPGraph *> graphs = _tdpgraph_map[tds->getInterproc()];
    }
