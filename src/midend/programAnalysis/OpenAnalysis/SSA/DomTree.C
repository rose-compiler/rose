// $Id: DomTree.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

//--------------------------------------------------------------------------------------------------------------------
// OpenAnalysis headers
#include "DomTree.h"

// STL headers
#include <algorithm>

#include <iostream>
using std::ostream;
using std::endl;

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Construct the dominator tree for the given directed graph.  This is (currently) done in the most straight-forward
    manner by iteratively solving data-flow equations for dominator sets.  While this dataflow framework is guaranteed
    rapid convergence, perhaps, a better (but more complicated) algorithm to implement would be Lengauer-Tarjan's that
    appears in "A Fast Algorithm for Finding Dominators in a Flowgraph", ACM Transactions on Programming Languages and
    Systems, Vol. 1, No. 1, July 1979, pages 121-141. */
DomTree::DomTree (DGraph& g)
  : Tree()
{
  int i;
  cfg = &g;

  // first, populate the tree with nodes corresponding to the directed graph g and number them for easy reference
  int S = g.num_nodes();
  Node** tree_node_vector = new Node*[S];
  DGraph::Node** graph_node_vector = new DGraph::Node*[S];
  std::set<int>* dom_set = new std::set<int>[S];
  std::map<DGraph::Node*, int> graph_node_num;
  int count = 0;
  DGraph::DFSIterator dfs_iter(g);
  while ((bool)dfs_iter) {
    tree_node_vector[count] = new Node((DGraph::Node*)dfs_iter);
    graph_node_vector[count] = (DGraph::Node*)dfs_iter;
    graph_node_num[(DGraph::Node*)dfs_iter] = count;
    dom_tree_node[(DGraph::Node*)dfs_iter] = tree_node_vector[count];
    ++count;
    ++dfs_iter;
  }
  add(tree_node_vector[0]); // add the root node
  dom_set[0].insert(1); // initialize the root node's dom set

  // now propagate the dominator sets for each node iteratively until a fixed point is reached
  // but, we do this only for nodes that are actually reachable from the root ("count" nodes)
  bool changed = true;
  while (changed) {
    changed = false;
    std::set<int> tmp_dom;
    for (i=0; i < count; i++) {
      // intersect the dom sets of each of the predecessors
      tmp_dom.clear();
      DGraph::SourceNodesIterator src_iter(graph_node_vector[i]);
      while ((bool)src_iter && dom_set[graph_node_num[(DGraph::Node*)src_iter]].empty())
	++src_iter;
      if ((bool)src_iter) {
	int node_num = graph_node_num[(DGraph::Node*)src_iter];
	tmp_dom = dom_set[node_num];
	++src_iter;
      }
      while ((bool)src_iter) {
	int node_num = graph_node_num[(DGraph::Node*)src_iter];
	if (!dom_set[node_num].empty()) {
	  std::set<int> tmp;
	  std::set_intersection(tmp_dom.begin(), tmp_dom.end(), dom_set[node_num].begin(), dom_set[node_num].end(),
                                std::inserter(tmp, tmp.begin()));
          tmp_dom = tmp;
	}
	++src_iter;
      }
      tmp_dom.insert(i);
      if (!(tmp_dom == dom_set[i])) {
	changed = true;
	dom_set[i] = tmp_dom;
      }
    }
  }

  // finally, find the immediate dominator of each node and insert appropriate edges
  for (i=0; i < count; i++) {
    std::set<int>::iterator ds_iter = dom_set[i].begin();
    int parent_of_i = -1;
    while (ds_iter != dom_set[i].end()) {
      int val = *ds_iter;
      if ((parent_of_i < val) && (val != i))
	parent_of_i = val;
      ++ds_iter;
    }
    if (parent_of_i >= 0)
      add(new Edge(tree_node_vector[parent_of_i], tree_node_vector[i]));
  }

  // free the temporary space
  delete[] tree_node_vector;
  delete[] graph_node_vector;
  delete[] dom_set;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Dominance frontier for the dominator tree is computed simply by looking at each branch point in the directed graph
    (a node that has more than one predecessor) and adding such a node to the dominance frontier of the chain of
    immediate dominators of each of its predecessors terminating at the node's immediate dominator.  The function uses
    the following algorithm, from the book "Engineering a Compiler", by Keith D. Cooper and Linda Torczon, chapter
    "Data-flow Analysis".

    <PRE>
    for all nodes b
      if the number of predecessors of b > 1
          for all predecessors, p, of b
              runner <-- p
                 while runner != IMMEDIATE_DOMINATOR(b)
                       add b to DOMINANCE_FRONTIER(runner)
                       runner = IMMEDIATE_DOMINATOR(runner)
    </PRE>
*/
void
DomTree::compute_dominance_frontiers ()
{
  DGraph::NodesIterator nodes_iter(*cfg);
  while ((bool)nodes_iter) {
    DGraph::Node* b = nodes_iter;
    if (b->num_incoming() > 1) {
      DGraph::SourceNodesIterator p(b);
      while ((bool)p) {
	DGraph::Node* runner = p;
	// this parent may be unreachable in the control flow graph and, hence, may have no corresponding Dominator
	// Tree node
	if (dom_tree_node.find(p) != dom_tree_node.end()) {
	  Node* b_dom_tree_node = dom_tree_node[b];
	  Node* runner_dom_tree_node = dom_tree_node[runner];
	  while (runner_dom_tree_node != b_dom_tree_node->parent()) {
	    if (runner_dom_tree_node->dom_front.find(b_dom_tree_node) == runner_dom_tree_node->dom_front.end())
	      runner_dom_tree_node->dom_front.insert(b_dom_tree_node);
	    runner = dynamic_cast<Node*>(runner_dom_tree_node->parent())->graph_node_ptr;
	    runner_dom_tree_node = dom_tree_node[runner];
	  }
	}
	++p;
      }
    }
    ++nodes_iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
DomTree::dump (ostream& os)
{
  NodesIterator nodes_iter(*this);
  while ((bool)nodes_iter) {
    Node* n = dynamic_cast<Node*>((Tree::Node*)nodes_iter);
    os << "[";
    if (n->parent() != 0)
      dynamic_cast<Node*>(n->parent())->graph_node_ptr->dump(os);
    else
      os << " ";
    os << "] --> ";
    n->graph_node_ptr->dump(os);
    os << " --> {";
    ChildNodesIterator child_iter(n);
    if ((bool)child_iter) {
      dynamic_cast<Node*>((Tree::Node*)child_iter)->graph_node_ptr->dump(os);
      ++child_iter;
      while ((bool)child_iter) {
	os << ", ";
	(dynamic_cast<Node*>((Tree::Node*)child_iter))->graph_node_ptr->dump(os);
	++child_iter;
      }
    }
    os << "}  DF = {";
    DomFrontIterator front_iter(n);
    if ((bool)front_iter) {
      ((DGraph::Node*)front_iter)->dump(os);
      ++front_iter;
      while ((bool)front_iter) {
	os << ", ";
	((DGraph::Node*)front_iter)->dump(os);
	++front_iter;
      }
    }
    os << "}" << endl;
    ++nodes_iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------
