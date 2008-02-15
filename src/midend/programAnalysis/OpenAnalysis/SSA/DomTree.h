// $Id: DomTree.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

#ifndef DOMTREE_H
#define DOMTREE_H


//--------------------------------------------------------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/Tree.h>
#include <OpenAnalysis/Utils/DGraph.h>

// STL headers
#include <set>
#include <map>
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
class DomTree : public Tree {
public:
  DomTree (DGraph& g);
  ~DomTree () {}

  //------------------------------------------------------------------------------------------------------------------
  class DomFrontIterator;
  class Node : public Tree::Node {
  public:
    Node (DGraph::Node* n) : Tree::Node() { graph_node_ptr = n; }
    ~Node () {}
    DGraph::Node* graph_node () { return graph_node_ptr; }
  private:
    DGraph::Node* graph_node_ptr;
    std::set<Node*> dom_front;
    friend class DomTree;
    friend class DomTree::DomFrontIterator;
  };
  //------------------------------------------------------------------------------------------------------------------
  class DomFrontIterator : public Iterator {
  public:
    DomFrontIterator (Node* n) : Iterator () { center = n; if (center != 0) iter = center->dom_front.begin(); }
    ~DomFrontIterator () {}
    void operator ++ () { if ((center != 0) && (iter != center->dom_front.end())) ++iter; }
    operator bool () { return (center != 0) && (iter != center->dom_front.end()); }
    operator Node* () { return *iter; }
    operator DGraph::Node* () { Node* n = *iter;  return n->graph_node_ptr; }
  private:
    Node* center;
    std::set<Node*>::iterator iter;
  };
  //-------------------------------------------------------------------------------------------------------------------

  Node* domtree_node (DGraph::Node* n) { return dom_tree_node[n]; }
  void compute_dominance_frontiers ();
  void dump (ostream&);

private:
  DGraph* cfg;
  std::map<DGraph::Node*, Node*> dom_tree_node;
};
//--------------------------------------------------------------------------------------------------------------------

#endif
