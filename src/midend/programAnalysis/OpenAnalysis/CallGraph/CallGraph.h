// $Id: CallGraph.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

#ifndef CallGraph_H
#define CallGraph_H

//--------------------------------------------------------------------------------------------------------------------
// standard headers
#ifdef NO_STD_CHEADERS
# include <string.h>
#else
# include <cstring>
#endif

// STL headers
#include <list>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/DGraph.h>
#include <OpenAnalysis/Interface/IRInterface.h>

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** CallGraph is a DGraph (directed graph) with enhanced nodes and
    edges.  Each node corresponds to a function definition or
    reference (or both).  Each node contains a list of places where it
    is defined or referenced. */
class CallGraph : public DGraph {
public:
  class Node;

  // Changes here must be also reflected in CallGraph.C:edgeTypeToString.
  enum EdgeType { NORMAL_EDGE = 0 };
  
public:
  CallGraph (IRInterface &_ir, IRProcIterator *fi, const SymHandle& name);
  virtual ~CallGraph ();
  
  //-------------------------------------
  // CallGraph information access
  //-------------------------------------
  Node *Entry() { return entry; }; // FIXME
  Node *Exit() { return exit; };
  IRInterface &GetIRInterface() { return ir; }

  //--------------------------------------------------------
  // Exceptions
  //--------------------------------------------------------
  class CallGraphException : public Exception {
  public:
    void report (std::ostream& os) const { os << "E! Unexpected." << std::endl; }
  };
  //--------------------------------------------------------

  class NodeUsesIterator;
  //--------------------------------------------------------
  class Node : public DGraph::Node {
  public:
    Node () 
      : DGraph::Node(), label(++label_count), sym(0), def(0) { }
    Node (SymHandle s) 
      : DGraph::Node(), label(++label_count), sym(s), def(0) { }
    ~Node () { }
    
    unsigned int getID () { return label; }

    bool IsDefined() { return (def != 0); }
    bool IsUsed() { return (uses.size() != 0); }
    
    ProcHandle GetDef() { return def; }

    void dump(std::ostream& os);
    void shortdump(CallGraph* cgraph, std::ostream& os);
    void longdump(CallGraph* cgraph, std::ostream& os);
    
    friend class CallGraph::NodeUsesIterator;

  public: // be careful
    void add_def(ProcHandle h) { def = h; }
    void add_use(ExprHandle h) { uses.push_back(h); }
    
  private:
    static unsigned int label_count;
    unsigned int label;
    
    //std::string name; // should be identical accross def and uses
    SymHandle sym;      // should be identical accross def and uses
    ProcHandle def;             // where this function is defined (if any)
    std::list<ExprHandle> uses; // places this function is used (if any)
  };
  
  //--------------------------------------------------------
  class Edge : public DGraph::Edge {
  public:
    Edge (Node* n1, Node* n2, EdgeType type);
    ~Edge () {}
    EdgeType getType() { return type; }
    void dump (std::ostream& os);
  private:
    EdgeType type;
  };  
  //--------------------------------------------------------
  
  class NodeUsesIterator : public Iterator {
  public:
    NodeUsesIterator (Node* node_) { node = node_; iter = node->uses.begin(); }
    ~NodeUsesIterator () {}
    void operator ++ () { if (iter != node->uses.end()) { ++iter; } }
    operator bool () { return (iter != node->uses.end()); }
    operator ExprHandle () { return *iter; }
  private:
    Node* node;
    std::list<ExprHandle>::iterator iter;
  };

  
  void dump (std::ostream& os);
  const SymHandle& subprog_name () { return name; }

private:
  class NodeLabelListIterator;
  friend class NodeLabelListIterator;

  void connect (Node* src, Node* dst, EdgeType type) {
    add (new Edge (src, dst, type));
  }
  void disconnect (Edge* e) { remove(e); }

  void build_graph(IRProcIterator* funcIter);
  void build_graph_from_block(CallGraph::Node* currProcNode, 
                              IRStmtIterator* stmtIter);

  CallGraph::Node* FindOrAddNode(SymHandle sym);

  typedef std::map<SymHandle, CallGraph::Node*> SymToNodeMap;

private:
  SymHandle name;
  IRInterface &ir;
  CallGraph::Node *entry; // FIXME
  CallGraph::Node *exit;
  
  SymToNodeMap symToNodeMap;
  
};
//--------------------------------------------------------------------------------------------------------------------

#endif
