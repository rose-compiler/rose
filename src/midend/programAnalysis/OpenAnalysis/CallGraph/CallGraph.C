// $Id: CallGraph.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
// -*-C++-*-
// * BeginRiceCopyright *****************************************************
// 
// Copyright ((c)) 2002, Rice University 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of Rice University (RICE) nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// This software is provided by RICE and contributors "as is" and any
// express or implied warranties, including, but not limited to, the
// implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall RICE or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or
// consequential damages (including, but not limited to, procurement of
// substitute goods or services; loss of use, data, or profits; or
// business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence
// or otherwise) arising in any way out of the use of this software, even
// if advised of the possibility of such damage. 
// 
// ******************************************************* EndRiceCopyright *

// Best seen in 120-column wide window (or print in landscape mode).
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

// standard headers

#ifdef NO_STD_CHEADERS
# include <stdlib.h>
# include <string.h>
# include <assert.h>
#else
# include <cstdlib>
# include <cstring>
# include <cassert>
using namespace std; // For compatibility with non-std C headers
#endif

#include <iostream>
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;

// Mint headers
#include "CallGraph.h"

unsigned int CallGraph::Node::label_count = 0; // First node will be numbered 1

//--------------------------------------------------------------------------------------------------------------------
CallGraph::CallGraph (IRInterface &_ir, IRProcIterator *procInter, 
		      const SymHandle& _name) 
  : ir(_ir)
{
  entry = exit = NULL;
  name = _name;
  build_graph(procInter);
}

CallGraph::~CallGraph()
{
  entry = NULL;
  exit = NULL;
}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
CallGraph::Edge::Edge (CallGraph::Node* n1, CallGraph::Node* n2, 
                       EdgeType _type) 
  : DGraph::Edge(n1, n2)
{
  type = _type;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
static const char *edgeTypeToString[] = { 
  "NORMAL_EDGE" // FIXME (make CFG's version static)
};

void CallGraph::Edge::dump(ostream& os)
{
  os << edgeTypeToString[type];
}
//--------------------------------------------------------------------------------------------------------------------

void
CallGraph::build_graph(IRProcIterator* procIter)
{
  // Iterate over all the procedures in the program
  for ( ; procIter->IsValid(); ++(*procIter)) { 
    
    // Create a node for this procedure
    ProcHandle currProc = procIter->Current();
    SymHandle currProcSym = ir.GetProcSymHandle(currProc);
    CallGraph::Node* currProcNode = FindOrAddNode(currProcSym);
    currProcNode->add_def(currProc); 
    
    // Iterate over the statements of this procedure
    IRStmtIterator *stmtIter = ir.ProcBody(currProc);
    build_graph_from_block(currProcNode, stmtIter);
    delete stmtIter;
  }
}


// Recursively build all statement blocks within a procedure
void
CallGraph::build_graph_from_block(CallGraph::Node* currProcNode,
				  IRStmtIterator* stmtIter)
{
  // Iterate over the statements of this block adding procedure references
  for ( ; stmtIter->IsValid(); ++(*stmtIter)) {

    StmtHandle stmt = stmtIter->Current();

    switch (ir.GetStmtType(stmt)) {
    case COMPOUND: {
      IRStmtIterator* it = ir.GetFirstInCompound(stmt);
      build_graph_from_block(currProcNode, it);
      delete it;
      break;
    }
    case LOOP:
    case END_TESTED_LOOP: {
      IRStmtIterator* it = ir.LoopBody(stmt);
      build_graph_from_block(currProcNode, it);
      delete it;
      break;
    }
    case STRUCT_TWOWAY_CONDITIONAL: {
      IRStmtIterator *it1 = ir.TrueBody(stmt), *it2 = ir.ElseBody(stmt);
      build_graph_from_block(currProcNode, it1);
      build_graph_from_block(currProcNode, it2);
      delete it1;
      delete it2;
      break;
    }
    case STRUCT_MULTIWAY_CONDITIONAL: {
      int numcases = ir.NumMultiCases(stmt);
      for (int i = 0; i < numcases; ++i) {
	IRStmtIterator* it = ir.MultiBody(stmt, i);
	build_graph_from_block(currProcNode, it);
	delete it;
      }
      IRStmtIterator* it = ir.GetMultiCatchall(stmt);
      build_graph_from_block(currProcNode, it);
      delete it;
      break;
    }
    case USTRUCT_TWOWAY_CONDITIONAL_T:
    case USTRUCT_TWOWAY_CONDITIONAL_F:
    case USTRUCT_MULTIWAY_CONDITIONAL: {
      break;
    }
    default: {
      // Iterate over procedure calls of a statement
      IRCallsiteIterator* callsiteIt = ir.GetCallsites(stmt);
      for ( ; callsiteIt->IsValid(); ++(*callsiteIt)) {
	ExprHandle expr = callsiteIt->Current();
	SymHandle sym = ir.GetSymHandle(expr);
	
	// Add a node (if nonexistent) and edge
	CallGraph::Node* node = FindOrAddNode(sym);
	node->add_use(expr);
	connect(currProcNode, node, NORMAL_EDGE);
      }
      delete callsiteIt;
      break;
      }
    }
    
  }
}


CallGraph::Node* 
CallGraph::FindOrAddNode(SymHandle sym)
{
  CallGraph::Node* node = NULL;

  SymToNodeMap::iterator it = symToNodeMap.find(sym);
  if (it == symToNodeMap.end()) {
    node = new CallGraph::Node(sym);
    add(node);
    symToNodeMap.insert(SymToNodeMap::value_type(sym, node));
  } else {
    node = (*it).second;
  }
  assert(node != NULL);
  
  return node;
}

//--------------------------------------------------------------------------------------------------------------------
void
CallGraph::dump (ostream& os)
{
  os << "===== CallGraph: " << ir.GetSymNameFromSymHandle(name) << " =====\n"
     << endl;
  
  // print the contents of all the nodes
  NodesIterator nodes_iter(*this);
  for ( ; (bool)nodes_iter; ++nodes_iter) {
    CallGraph::Node* node = 
      dynamic_cast<CallGraph::Node*>((DGraph::Node*)nodes_iter);
    node->longdump(this, os);
    os << endl;
  }
  
  os << "====================" << endl;

}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
CallGraph::Node::dump (ostream& os)
{
  os << sym;
}

void
CallGraph::Node::shortdump (CallGraph *cgraph, ostream& os)
{
  IRInterface& ir = cgraph->GetIRInterface();
  if (sym == 0) {
    os << "<no-symbol>";
  } else {
    os << ir.GetSymNameFromSymHandle(sym);
  }
  os << " " << (IsDefined() ? "[defined]" : "[referenced]");
}

void
CallGraph::Node::longdump (CallGraph *cgraph, ostream& os)
{
  // print the node ID
  IRInterface& ir = cgraph->GetIRInterface();
  os << "CallGraph Node: ";
  shortdump(cgraph, os);
  
  if (num_incoming() == 0)
    os << " (root)";
  if (cgraph->Entry() == this) {
    os << " [ENTRY]";
  } else if (cgraph->Exit() == this) {
    os << " [EXIT]"; 
  }
  os << endl;
  
  // print the source(s)
  unsigned int count = 0;
  SourceNodesIterator srcIt(this);
  for ( ; (bool)srcIt; ++srcIt, ++count) {
    Node* node = dynamic_cast<Node*>((DGraph::Node*)srcIt);
    if (count == 0) { os << " <-- ("; }
    else            { os << ", "; }

    node->shortdump(cgraph, os);
  }
  if (count > 0) { os << ")" << endl; }
  
  // print the sink(s)
  count = 0;
  OutgoingEdgesIterator outIt(this);
  for ( ; (bool)outIt; ++outIt, ++count) {
    Edge* edge = dynamic_cast<Edge*>((DGraph::Edge*)outIt);
    Node* node = dynamic_cast<Node*>(edge->sink());
    if (count == 0) { os << " --> ("; } 
    else            { os << ", "; }
    
    node->shortdump(cgraph, os);
    os << " [";
    edge->dump(os);
    os << "]";
  }
  if (count > 0) { os << ")" << endl; }
}
//--------------------------------------------------------------------------------------------------------------------
