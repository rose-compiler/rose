// $Id: CFG.h,v 1.3 2008/01/08 02:56:40 dquinlan Exp $
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
// This file is part of Mint.
// Arun Chauhan (achauhan@cs.rice.edu), Dept of Computer Science, Rice University, 2001.
//--------------------------------------------------------------------------------------------------------------------

#ifndef CFG_H
#define CFG_H

//--------------------------------------------------------------------------------------------------------------------
// standard headers
#ifdef NO_STD_CHEADERS
# include <string.h>
#else
# include <cstring>
#endif

// STL headers
#include <list>
#include <set>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/DGraph.h>
#include <OpenAnalysis/Interface/IRInterface.h>

class Worklist;

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** CFG is a DGraph (directed graph) with enhanced nodes and edges.  Each node in the CFG points to a list of 
    statements that together represent a basic block.  The entire program would be represented by a set of CFGs,
    one for each subroutine, and one for the main program.
*/
class CFG : public DGraph {
public:
  class Node;
  class NonLocalsIterator;
  class DefBlocksIterator;
  friend class Node;
  friend class NonLocalsIterator;
  friend class DefBlocksIterator;

  // Changes here must be also reflected in CFG.C:edgeTypeToString.
  enum EdgeType { TRUE_EDGE = 0, FALLTHROUGH_EDGE, FALSE_EDGE,
		  BACK_EDGE, MULTIWAY_EDGE, BREAK_EDGE,
		  CONTINUE_EDGE, RETURN_EDGE };

private:
  //------------------------------------------------------------------------------------------------------------------
  // Structure used for comparing node texts in an STL set of leaf handles.
#if 0
  // FIXME: eraxxon: This is used for ordering a LeafHandle set.
  // Since LeafHandles have to be a builtin scalar type why not just
  // use the default ordering?
  struct ltnode {
    bool operator() (const LeafHandle n1, const LeafHandle n2) const {
# if 0
      return strcmp(IRGetSymHandleFromLeafHandle(n1).name(),
                    IRGetSymHandleFromLeafHandle(n2).name()) < 0;
# else
// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
//    using namespace::std; // For compatibility with non-std C headers
      return strcmp(IRGetSymNameFromLeafHandle(n1),
                    IRGetSymNameFromLeafHandle(n2)) < 0;
# endif
    }
  };
#endif

  //------------------------------------------------------------------------------------------------------------------

public:
  CFG (IRInterface &_ir, IRStmtIterator *si, SymHandle name, bool return_statements_allowed = true);

  virtual ~CFG ();

  //-------------------------------------
  // CFG information access
  //-------------------------------------
  Node *Entry() { return entry; };
  Node *Exit() { return exit; };
  IRInterface &GetIRInterface() { return ir; }

  //------------------------------------------------------------------------------------------------------------------
  // Exceptions
  //------------------------------------------------------------------------------------------------------------------
  class Unexpected_Break : public Exception {
  public:
    void report (std::ostream& os) const { os << "E!  Unexpected break statement." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------
  class Unexpected_Return : public Exception {
  public:
    void report (std::ostream& os) const { os << "E!  Unexpected return statement." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------
  class Unexpected_Continue : public Exception {
  public:
    void report (std::ostream& os) const { os << "E!  Unexpected continue statement." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------

  class NodeStatementsIterator;
  //------------------------------------------------------------------------------------------------------------------
  class Node : public DGraph::Node {
  public:
    Node () : DGraph::Node() { label = ++label_count; }
    Node (StmtHandle n) : DGraph::Node() { 
      stmt_list.push_back(n); label = ++label_count; 
    }
    ~Node () { stmt_list.clear(); }
    
    unsigned int getID () { return label; }
    
    void add (StmtHandle h) { stmt_list.push_back(h); }
    StmtHandle erase (StmtHandle h); // Careful: linear time!
    unsigned int size () { return stmt_list.size(); }
    bool empty () { return stmt_list.empty(); }
    
    void addEndExpr (ExprHandle expr) { end = expr; }
    ExprHandle getEndExpr () { return end; }
    
    void split(StmtHandle splitPoint, Node* newBlock);
    
    void dump (std::ostream& os) { os << label; }
    void longdump (CFG *, std::ostream& os);
    void longdump (CFG * _cfg) { longdump(_cfg, std::cout); } 
    
    friend class CFG;
    friend class CFG::NodeStatementsIterator;
  private:
    static unsigned int label_count;
    unsigned int label;
    std::list<StmtHandle> stmt_list;
    ExprHandle end;
  };
  //------------------------------------------------------------------------------------------------------------------
  class Edge : public DGraph::Edge {
  public:
    Edge (Node* n1, Node* n2, EdgeType type, ExprHandle expr);
    ~Edge () {}
    EdgeType getType() { return type; }
    void dump (std::ostream& os);
    void dump () { dump(std::cout); }
  private:
    EdgeType type;
    ExprHandle expr;
  };  
  //------------------------------------------------------------------------------------------------------------------
  typedef std::list<Node*> NodeList;
  class NodeListIterator : public Iterator {
  public:
    NodeListIterator (NodeList* nl) { list = nl; iter = list->begin(); }
    virtual ~NodeListIterator () {}
    void operator ++ () { if (iter != list->end()) ++iter; }
    operator bool () { return (iter != list->end()); }
    operator Node* () { return *iter; }
  private:
    NodeList* list;
    std::list<Node*>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  //------------------------------------
  // enumerate nodes in DFS order
  //------------------------------------
  class DFSIterator : public DGraph::DFSIterator {
  public:
    DFSIterator (CFG  &g) : DGraph::DFSIterator(g) {}
    virtual ~DFSIterator () {}
    operator Node* () { return dynamic_cast<CFG::Node*>(p); }
    operator bool () { return (bool)p; }
  };
  //------------------------------------------------------------------------------------------------------------------
  class NodeStatementsIterator : public Iterator {
  public:
    NodeStatementsIterator (Node* node) { center = node; iter = center->stmt_list.begin(); }
    ~NodeStatementsIterator () {}
    void operator ++ () { if (iter != center->stmt_list.end()) ++iter; }
    operator bool () { return (iter != center->stmt_list.end()); }
    operator StmtHandle () { return *iter; }
  private:
    Node* center;
    std::list<StmtHandle>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  class NonLocalsIterator : public Iterator {
  public:
    NonLocalsIterator (CFG& c) { cfg = &c; iter = cfg->non_locals.begin(); }
    void operator++ () { ++iter; }
    operator bool () { return (iter != cfg->non_locals.end()); }
    operator LeafHandle () { return *iter; }
  private:
    CFG* cfg;
    std::set<LeafHandle/*, ltnode*/>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  class DefBlocksIterator : public Iterator {
  public:
    DefBlocksIterator (CFG& cfg, SymHandle name) { blk_set = &(cfg.def_blocks_set[name]); iter = blk_set->begin(); }
    void operator++ () { ++iter; }
    operator bool () { return (iter != blk_set->end()); }
    operator Node* () { return *iter; }
  private:
    std::set<Node*>* blk_set;
    std::set<Node*>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  void dump (std::ostream& os);
  void dump () { dump(std::cout); }
  SymHandle subprog_name () { return name; }
  void compute_uses_sets ();
  void compute_defs_sets ();

private:
  class NodeLabelListIterator;
  friend class NodeLabelListIterator;
  class NodeLabel {
  public:
    NodeLabel (Node* _n, EdgeType _et, ExprHandle _eh = 0) {
      n = _n; eh = _eh; et = _et;
    }
    Node* n;
    EdgeType et;
    ExprHandle eh;
    friend class CFG;
  };
  typedef std::list<NodeLabel> NodeLabelList;
  class NodeLabelListIterator : public Iterator {
  public:
    NodeLabelListIterator (NodeLabelList* nl) { list = nl; iter = list->begin(); }
    virtual ~NodeLabelListIterator () {}
    void operator ++ () { if (iter != list->end()) ++iter; }
    operator bool () { return (iter != list->end()); }
    operator NodeLabel () { return *iter; }
  private:
    NodeLabelList* list;
    std::list<NodeLabel>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  IRStmtType build_block (Node* prev_node, IRStmtIterator *si,
    NodeLabelList& exit_nodes, NodeLabelList* break_nodes,
    NodeLabelList* return_nodes, NodeLabelList* continue_nodes);

  IRStmtType build_stmt (Node* prev_node, StmtHandle, NodeLabelList& exit_nodes, NodeLabelList* break_nodes,
			    NodeLabelList* return_nodes, NodeLabelList* continue_nodes)
    throw (Unexpected_Break, Unexpected_Return, Unexpected_Continue);
  IRStmtType build_CFG_loop (Node* prev_node, StmtHandle th, NodeLabelList& exit_nodes,
                             NodeLabelList* return_nodes);
  IRStmtType build_CFG_end_tested_loop (Node* prev_node, StmtHandle th, NodeLabelList& exit_nodes,
				        NodeLabelList* return_nodes);
  IRStmtType build_CFG_twoway_branch (Node* prev_node, StmtHandle th, NodeLabelList& exit_nodes,
					  NodeLabelList* break_nodes, NodeLabelList* return_nodes,
                                          NodeLabelList* continue_nodes);
  IRStmtType build_CFG_multiway_branch (Node* prev_node, StmtHandle th, NodeLabelList& exit_nodes,
					    NodeLabelList* break_nodes, NodeLabelList* return_nodes,
                                            NodeLabelList* continue_nodes);
  IRStmtType build_CFG_multiway_branch_with_fallthrough (Node* prev_node, StmtHandle th, NodeLabelList& exit_nodes,
					    NodeLabelList* return_nodes, NodeLabelList* continue_nodes);
  IRStmtType build_CFG_unconditional_jump (Node* prev_node, StmtHandle stmt);
  IRStmtType build_CFG_unconditional_jump_i (Node* prev_node, StmtHandle stmt);
  IRStmtType build_CFG_ustruct_twoway_branch (Node* prev_node, StmtHandle stmt, CFG::NodeLabelList& exit_nodes);
  IRStmtType build_CFG_ustruct_multiway_branch (Node* prev_node, StmtHandle stmt);

  //******************************************
  // Support for building cfgs with delay slots.
  //******************************************
  void HandleDelayedBranches();
  bool isInternalBranch(StmtHandle);
  Node* getLabelBlock(StmtLabel);
  void processBlock(CFG::Node* );
  void createBasicCFG();
  void processBlock();

public: 
  Node* splitBlock(Node*, StmtHandle /* CFG::NodeStatementsIterator */);

  void connect (Node* src, Node* dst, EdgeType type) {
    add (new Edge (src, dst, type, 0));
  }
  void connect (Node* src, Node* dst, EdgeType type, ExprHandle expr) {
    add (new Edge (src, dst, type, expr));
  }

  void connect (Node*, NodeLabelList&);
  void connect (NodeLabelList&, Node*);
  void disconnect (Edge* e) { remove(e); }
  CFG::Node* node_from_label (StmtLabel);

private:
  SymHandle name;
  std::set<LeafHandle/*, ltnode*/> non_locals;
  std::map<SymHandle, std::set<Node*> /* , Name::CompStruct */> def_blocks_set;

  IRInterface &ir;
  CFG::Node *entry;
  CFG::Node *exit;
  std::map<StmtLabel, CFG::Node*> label_to_node_map;

private:
  //------------------------------------------
  // data structures for handling delay slots
  //------------------------------------------
  Worklist *the_worklist;
  std::map<CFG::Node*, CFG::Node*> fallThrough;
};
//--------------------------------------------------------------------------------------------------------------------

#endif
