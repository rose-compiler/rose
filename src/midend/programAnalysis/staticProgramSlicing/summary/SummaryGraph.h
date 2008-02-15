#ifndef _SUMMARYGRAPH_H_
#define _SUMMARYGRAPH_H_

#include "DependenceGraph.h"

//! a map from a DependenceNode representing an attribute to the DependenceNodes that define it
typedef map<DependenceNode *, set<DependenceNode *> > AttributeEquations;

/*! \class AttributeDefinitionEquation

This class represents the attribute-definition equations, as discussed
in Horwitz et al. These basically capture dependences between the
"attribute nodes" (i.e. the parameter nodes and argument nodes for
function calls) of a PDG. They are used later in the construction of
summary edges.

*/
class AttributeDefinitionEquations {
  
public:
  
  /*! \brief Constructor for AttributeDefinitionEquations
    
  Params:
    - DependenceNode * attribute: The attribute we want to construct a definition equation for
    - SystemDependenceGraph * sdg: the SDG which contains the interprocedural info required for building the equation
  */
  AttributeDefinitionEquations(SystemDependenceGraph * sdg);
  
  //! output the equations to os
  void writeOut(ostream &os);

  //! get the attribute equations for a given pdg
  AttributeEquations getEquations(ProgramDependenceGraph * pdg);

private:
  
  //! determine the attribute definition equations
  void _makeEquations();
  
  //! generate the attribute defs for a specific PDG
  void _generateEquationsForPDG(ProgramDependenceGraph * pdg);
  
  /* \brief get the appropriate attribute node associated with this node
     
  Params:
    - DependenceNode * node: The node we want to get the corresponding attribute for

  Returns: the FORMAL node that should be used as an attribute

  We use the FORMALIN/OUT nodes from the PDGs' _interprocedural
  objects as stand ins for the attributes, so we use this function to
  retrieve those nodes
  */
  DependenceNode * _getAttributeNode(DependenceNode * node);
  
  //! The SystemDependenceGraph we are building the def equations for
  SystemDependenceGraph * _sdg;
    
  //! a map from PDGs to the attribute equations for its attributes
  map<ProgramDependenceGraph *, AttributeEquations> _attribute_equations_map;
  
};

/*! \class TDSGraph
  
This class captures the "covering subordinate characteristic graph,"
or the "Transitive Dependences among a Symbol's attributes" for a
given procedure. When fully constructed, this graph will
capture the [interprocedural] transitive dependences between the
attributes in the procedure. (see paper by Horwitz et al).

Note that although technically these graphs are associated with a
symbol in the attribute grammar, there is a one-to-one correspondence
between the symbols of the grammar and the procedures in the
program. Thus, the TDSGraphs are effectively associated with each
procedure.
*/
class TDSGraph : public DependenceGraph {
  
public:
      
  /*! \brief constructor for TDSGraph
    
  Params:
    - InterproceduralInfo * ii: The interprocedural info for the procedure we are building the TDSGraph for
  */
  TDSGraph(InterproceduralInfo * ii);

  InterproceduralInfo * getInterproc() {return _inter;}

  //! returns true if there are unmarked edges in the graph
  bool hasUnmarked();

  //! gets an unmarked edge
  Edge popUnmarked();

  //! adds an unmarked edge
  void pushUnmarked(Edge e);

private:

  InterproceduralInfo * _inter;

  //! A list of unmarked edges
  set<Edge> unmarked;

};

/* \class TDPGraph

This class in an auxilliary class used during the construction of the
TDSGraph. This is the "Transitive Dependences in a Product" graph.

While there is a one-to-one correspondence between productions and
procedures (i.e.each procedure generates one production in the Linkage
Grammar), there is a difference between these and TDSGraphs. TDSGraphs
contain every attribute for a given procedure (i.e. the formal-in and
formal-out nodes), while TDPGraphs contain the attributes used in the
Linkage Grammar production for the procedure (so the attributes might
not all be from the same procedure).
*/

class TDPGraph : public DependenceGraph {

public:
  /*! \brief constructor for TDPGraph
    
  Params: 
    - AttributeEquations ae: The attribute equations for the attributes in the production
  */
  TDPGraph(AttributeEquations ae);

  /*! \brief adds an edge and performs a closure operation

  Params:
    - DependenceNode * from: the source of the edge to add
    - DependenceNode * to: the sink of the edge to add

  Side effects: Adds Edge(from, to) to the graph. It also adds any
    edges required to transitively close the graph.

  Returns: The set of edges added to produce transitively closed graph

  Note: from and to are expected to be nodes in the graph (as opposed
  to nodes which have representations in the graph). The return edges,
  however, are the root nodes.
  */
  set<Edge> AddEdgeAndClose(DependenceNode * from, DependenceNode * to);

private:

  /*! \brief returns the set of edges that would transitively close the graph if the edge were added

  Params:
    - Edge e: The edge we are adding to the graph

  Returns: the set of edges that would need to be added to transitively close the graph
  */
  set<Edge> _closure(Edge e);

  DependenceNode * _getAttributeNode(DependenceNode * node);
};

/* \class SummaryGraph

This class is a DependenceGraph which holds summary edges between the
various nodes in an SystemDependenceGraph. The nodes of the graph
reflect the "summary nodes" (i.e. actual-in/out nodes) of the
SDG. When merged with the SDG, it results in an SDG with summary edges.

*/
class SummaryGraph : public DependenceGraph {

public:
  /*! \brief constructor for SummaryGraph

  Params:
    - SystemDependenceGraph * sdg: The SystemDependenceGraph we want to produce summary edges for
  */
  SummaryGraph(SystemDependenceGraph * sdg);

private:

  //! initialize the TDS and TDP graphs
  void _initializeGraphs();

  //! after initializing the graphs, transfer the edges from the tdp graph to the tds graph
  void _initializeEdges();

  SystemDependenceGraph * _sdg;
  AttributeDefinitionEquations * _ade;

  //! maps attributes to the TDSGraphs that contain those attributes
  map<DependenceNode *, TDSGraph *> _tdsgraph_map;

  //! maps iis to the TDPGraphs that use their attributes
  map<InterproceduralInfo *, set<TDPGraph *> > _tdpgraph_map;

  //! set of TDSGraphs in the system
  set<TDSGraph *> _tdsgraphs;

  //! set of TDPGraphs in the system
  set<TDPGraph *> _tdpgraphs;

};

#endif
