// $Id: TarjanIntervals.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

//
//
// -- TarjanIntervals.h
//
//         Header file for Tarjan interval finder (see TarjanIntervals.C).
//
// History:
//   Mar. 1991 (phh):
//     Initial implementation for the D-System.
//   Feb. 2002 (John Mellor-Crummey):
//     Wrapped with C++ classes and cleaned-up.
//   Feb. 2002 (Jason Eckhardt):
//     Cleaned-up and incorporated into the OpenAnalysis system.
//
//

#ifndef TarjanIntervals_h
#define TarjanIntervals_h

//************************** System Include Files ***************************

//*************************** User Include Files ****************************


// OpenAnalysis headers.
#include "RIFG.h"
#include <OpenAnalysis/Utils/UnionFindUniverse.h>

enum RITarjType {RI_TARJ_NOTHING, RI_TARJ_ACYCLIC, RI_TARJ_INTERVAL, RI_TARJ_IRREDUCIBLE };

enum RITarjEdgeType { RI_TARJ_NORMAL, RI_TARJ_LOOP_ENTRY, RI_TARJ_IRRED_ENTRY, RI_TARJ_ITERATE };


class TarjWork;
class TarjTreeNode;


//*********************** Forward Class Definitions *************************

//************************** Variable Definitions ***************************

//************************ Enumeration Definitions **************************

//**************************** Class Definitions ****************************

class TarjanIntervals {
public:
  // Construct the Tarjan interval tree for the graph.
  TarjanIntervals(RIFG &_g);
  void Create();

  ~TarjanIntervals();


  // Updates the prenumbering of the tree.  Useful if some change
  // has been made to the cfg where one knows how to update the
  // interval tree (as when adding pre-header nodes).
  void Renumber();
  void Prenumber(int n);

  // Sorts the children of each loop header according to
  // topological order.
  // FIXME: Privatize this? It is always done when the tree
  // is created.
  void Sort();

  int IsFirst(RIFGNodeId id);
  int IsLast(RIFGNodeId id);
  int IsHeader(RIFGNodeId id);

  // Given an interval, return its nesting level.
  int Level(RIFGNodeId id);

  // Given an interval, return its unique index.
  int LoopIndex(RIFGNodeId id);

  // Given an interval, return its type (acyclic, interval, irreducible).
  RITarjType IntervalType(RIFGNodeId id);

  int Contains(RIFGNodeId a, RIFGNodeId b);

  // Return the least-common-ancestor of two nodes in the tree.
  RIFGNodeId LCA(RIFGNodeId a, RIFGNodeId b);

  // Return true if the edge is a backedge, false otherwise.
  int IsBackEdge(RIFGEdgeId e);

  // Return number of loops exited in traversing g edge from src to sink.
  int tarj_exits(RIFGNodeId src, RIFGNodeId sink);

  // Return outermost loop exited in traversing g edge from src to sink
  // (RIFG_NIL if no loops exited).
  RIFGNodeId tarj_loop_exited(RIFGNodeId src, RIFGNodeId sink);

  RITarjEdgeType tarj_edge_type(RIFGNodeId src, RIFGNodeId sink);

  // Given an interval header, return its first nested interval.
  RIFGNodeId TarjInners(RIFGNodeId id);

  // Given an interval, return its parent (header) interval.
  RIFGNodeId TarjOuter(RIFGNodeId id);

  // Given an interval, return its next sibling interval. 
  RIFGNodeId TarjNext(RIFGNodeId id);

  // Given an interval header, return its last nested interval.
  RIFGNodeId TarjInnersLast(RIFGNodeId id);

  // Pretty-print the interval tree.
  void Dump();

  // Obtain a pointer to tree.
  TarjTreeNode *GetTarjanTree();

private: // methods
  void Build();
  void FillPredLists();
  void GetTarjans();
  void FreeWork();
  void Init();
  void InitArrays();
  void DumpSubTree(int node, int indent);
  void ComputeIntervalIndex();
  void ComputeIntervalIndexSubTree(int node, int value);

  int FIND(int v);
  void UNION(int i, int j, int k);
  void DFS(RIFGNodeId n);

private: // data
  TarjWork *wk;
  UnionFindUniverse *uf;
  TarjTreeNode *tarj;

  // The graph to analyze.
  RIFG &g;

  // List of nodes in reverse topological order.
  std::list<RIFGNodeId> rev_top_list;

  // A map from RIFGNodeId to DFS number.
  std::map<RIFGNodeId, int> nodeid_to_dfnum_map;
};


#endif
