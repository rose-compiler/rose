// $Id: TarjanIntervals.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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
// -- TarjanIntervals
//
//       This file contains code to determine the Tarjan intervals (nested
//       strongly connected regions) of a flow graph.  Tarjan's original
//       algorithm is described in the following article:
//         Tarjan, R.E. "Testing flow graph reducibility"
//         J. of Computer & System Sciences 9, 355-365, 1974. 
//
//       Extensions to Tarjan's algorithm were made here to handle
//       reducible SCRs surrounded by or containing irreducible ones.
//       The extensions are described in the following article:
//         Havlak, P. "Nesting of reducible and irreducible loops"
//         ACM TOPLAS, July, 1997.
//
//       The result is returned as a tree of SCRs, where a parent SCR
//       contains its children.  A leaf SCR is a single node, which is not
//       really a loop unless the isCyclic flag is set.
//
//       Preconditions:
//       1. The underlying directed graph has a unique start node,
//          dominating all others in the graph and a unique exit node,
//          postdominating all others.
//      
//       The underlying node numbering scheme is not important.
//
// History:
//   Mar. 1991 (phh):
//     Initial implementation for the D-System.
//   Feb. 2002 (John Mellor-Crummey):
//     Wrapped with C++ classes and cleaned-up.
//   Feb. 2002 (Jason Eckhardt):
//     Further cleaned-up the code and rewrote some portions for
//     incorporation into the OpenAnalysis system.
//
//   A clean re-write in C++ would greatly enhance the style and clarity
//   of the code.
//


#include <iostream>
using std::cout;
#include <algorithm>     // for max
#include <map>
#include <list>

#ifdef NO_STD_CHEADERS
# include <string.h>
# include <stdio.h>
#else
# include <cstring>
# include <cstdio>
using namespace std; // For compatibility with non-std C headers
#endif

#include <unistd.h>

#include "TarjanIntervals.h"


#define ROOT_DFNUM      0
#define INVALID_DFNUM   -1
#define dfnum(v)                (nodeid_to_dfnum_map[v])


// There will be an array of TarjWorks, one for each node.  These
// are indexed by the DFS number of the node.  This way, there is no
// dependence on the node numbering of the underlying graph. 
class TarjWork {
public:
  TarjWork();
  ~TarjWork();

  int wk_vertex;        // map from DFS# of vertex to RIFGNodeId
  int wk_last;          // DFS # of vertex's last descendant
  int wk_header;        // header of the vertex's interval -- HIGHPT
  int wk_nextP;         // next member of P set == reachunder
  int wk_nextQ;         // next member of Q set == worklist
  bool wk_inP;          // test for membership in P == reachunder
  bool wk_isCyclic;     // has backedges -- to id singles
  bool wk_reducible;    // true if cyclic scr is reducible
  std::list<int> backPreds;     // List of preds that are backedges.
  std::list<int> nonBackPreds;  // List of preds that are non-backedges.
};


//
// Interval tree will consist of an array of TarjTreeNodes. These
// are indexed (internally) by the DFS number of the node.  This way,
// there is no dependence on the node numbering of the underlying graph. 
//
// There is also a map from RIFGNodeIds to DFS numbers, so
// that tree nodes can be accessed (by the client) through their
// node ids. 
//
class TarjTreeNode {
public:
  TarjTreeNode();
  RIFGNodeId nodeid;    // Associated RIFGNodeId.
  short level;          // nesting depth -- outermost loop is 1 
  RITarjType type;      // acyclic, interval or irreducible 
  int outer;            // DFS number of header of containing interval
  int inners;           // DFS number of header of first nested interval
  int next;             // DFS number of next nested header
  int prenum;           // preorder number
  int last;             // number of last descendent
  RIFGNodeId last_id;   // id of last descendent
  short loopIndex;      // unique id for intervals
};

#define TARJ_nodeid(name)       (tarj[name].nodeid)
#define TARJ_outer(name)        (tarj[name].outer)
#define TARJ_inners(name)       (tarj[name].inners)
#define TARJ_next(name)         (tarj[name].next)
#define TARJ_level(name)        (tarj[name].level)
#define TARJ_type(name)         (tarj[name].type)
#define TARJ_last(name)         (tarj[name].last)
#define TARJ_last_id(name)      (tarj[name].last_id)
#define TARJ_loopIndex(name)    (tarj[name].loopIndex)
#define TARJ_contains(a,b)      \
    ( ( tarj[a].prenum <= tarj[b].prenum ) && \
      ( tarj[b].prenum <= tarj[tarj[a].last].prenum ) \
    )



#define vertex(x)       (wk[x].wk_vertex)
#define TLast(x)        (wk[x].wk_last)
#define header(x)       (wk[x].wk_header)
#define nextP(x)        (wk[x].wk_nextP)
#define nextQ(x)        (wk[x].wk_nextQ)
#define inP(x)          (wk[x].wk_inP)
#define isCyclic(x)     (wk[x].wk_isCyclic)
#define reducible(x)    (wk[x].wk_reducible)
#define backPreds(x)    (wk[x].backPreds)
#define nonBackPreds(x) (wk[x].nonBackPreds)


static int n;           // next DFS preorder number
static int last_id;     // RIFGNodeId whose DFS preorder number is n


//
// is_backedge(a,b) returns true if a is a descendant of b in DFST
// (a and b are the DFS numbers of the corresponding vertices). 
//
// Note that this is in the local structures; corresponds somewhat
// to Contains(tarj,b,a) for the public structures.
//
//
#define is_backedge(a,b) ((b <= a) & (a <= TLast(b)))


TarjanIntervals::TarjanIntervals(RIFG &_g):g(_g)
{
  Create();
}


TarjanIntervals::~TarjanIntervals()
{
  if (tarj)
    delete [] tarj;
  if (uf)
    delete uf;
  if (wk)
    delete [] wk;
  rev_top_list.clear();
  nodeid_to_dfnum_map.clear();
}


// Construct tree of Tarjan intervals for RIFG & 'g' 
void TarjanIntervals::Create()
{
  RIFGNodeId root = g.GetRootNode();

  Init();
  DFS(root);
  FillPredLists();
  GetTarjans();
  Build();
  Sort();
  ComputeIntervalIndex();
  FreeWork();
}


//
// Sort the Tarjan Tree in topological ordering
//
// Feb. 2002 (jle) - Removed the call to GetTopologicalMap which computes
// a (breadth-first) topological ordering of the CFG. This is completely
// unnecessary (and wasteful) since by virtue of performing a DFS as part of
// interval building, we have a topological ordering available as a by-product.
// Further, the breadth-first version in GetTopologicalMap is clumsy (in this
// context) since it has to make call-backs to the tarjan code to determine
// if the current edge being examined is a back-edge (to avoid cycling).
//
void TarjanIntervals::Sort()
{
  RIFGNodeId gId;
  int parent;           // Tarj parent (loop header)

  // Now disconnect all "next" fields for all tarj nodes

  RIFGNodeIterator* ni = g.GetNodeIterator(g, RIFG::FORWARD);
  for ( ; (ni->Current()) != RIFG_NIL; (*ni)++) {
    gId = ni->Current();
    int gdfn = dfnum(gId);
    // gdfn will be invalid if the node gId was unreachable.
    if (gdfn != INVALID_DFNUM) {
      TARJ_inners(gdfn) = INVALID_DFNUM;
      TARJ_next(gdfn) = INVALID_DFNUM;
    }
  }
  delete ni;

  // The list is sorted in reverse topological order since each child
  // in the loop below is prepended to the list of children.
  std::list<RIFGNodeId>::iterator i;
  for (i = rev_top_list.begin(); i != rev_top_list.end(); i++) {
    gId = *i;
    if (gId != RIFG_NIL) {
      // Add new kid
      int gdfn = dfnum(gId);
      parent = TARJ_outer(gdfn);
      if (parent != INVALID_DFNUM) {
        TARJ_next(gdfn) = TARJ_inners(parent);
        TARJ_inners(parent) = gdfn;
      }
    }
  }

  Renumber();
} // end of tarj_sort()


TarjTreeNode *TarjanIntervals::GetTarjanTree()
{
  return tarj;
}


TarjTreeNode::TarjTreeNode()
{
  nodeid = RIFG_NIL;
  level = 0;            
  type = RI_TARJ_ACYCLIC;               
  outer = INVALID_DFNUM;        
  inners = INVALID_DFNUM;
  next = INVALID_DFNUM;
  prenum = -1;
  last = INVALID_DFNUM; 
  last_id= RIFG_NIL;
  loopIndex = RIFG_NIL;
}


TarjWork::TarjWork() 
{
  wk_vertex = RIFG_NIL; 
  wk_last = INVALID_DFNUM;      
  wk_header = ROOT_DFNUM; // should be ROOT_DFNUM, not RIFG_NIL;                
  wk_nextP = INVALID_DFNUM;             
  wk_nextQ = INVALID_DFNUM;
  wk_inP = false;
  wk_isCyclic = false;
  wk_reducible = true;  
}


TarjWork::~TarjWork() 
{
  backPreds.clear();
  nonBackPreds.clear();
}


//
// Allocate and initialize structures for algorithm, and UNION-FIND
//
void TarjanIntervals::Init()
{
  unsigned int g_size = g.HighWaterMarkNodeId() + 1;

  n = ROOT_DFNUM;

  //
  // Local work space
  //
  wk = new TarjWork[g_size];
  uf = new UnionFindUniverse(g_size);
  tarj = new TarjTreeNode[g_size];
  InitArrays();
}


//
// Initialize only nodes in current instance g
//
void TarjanIntervals::InitArrays()
{
  RIFGNodeIterator* ni = g.GetNodeIterator(g, RIFG::FORWARD);
  for ( ; (ni->Current()) != RIFG_NIL; (*ni)++) {
    int nid = ni->Current();
    dfnum(nid) = INVALID_DFNUM;
  }
  delete ni;
}


//
// Do depth first search on control flow graph to 
// initialize vertex[], dfnum[], last[]
//
void TarjanIntervals::DFS(RIFGNodeId v)
{
  vertex(n) = v;
  dfnum(v)  = n++;
 
  RIFGEdgeId succ;
  RIFGEdgeIterator* ei = g.GetEdgeIterator(g, v, RIFG::ED_OUTGOING);
  for (; (ei->Current()) != RIFG_NIL; (*ei)++) {
    succ = ei->Current();
    int son = g.GetEdgeSink(succ);
    if (dfnum(son) == INVALID_DFNUM)
      DFS(son);
  }
  delete ei;
  //
  // Equivalent to # of descendants -- number of last descendant
  //
  TLast(dfnum(v)) = n-1;
  rev_top_list.push_back(v);
}


// Fill in the backPreds and nonBackPreds lists for each node.
void TarjanIntervals::FillPredLists()
{
  for (int i = ROOT_DFNUM; i < n; i++) {
    RIFGEdgeIterator* ei = g.GetEdgeIterator(g, vertex(i), RIFG::ED_INCOMING);
    for ( ; (ei->Current()) != RIFG_NIL; (*ei)++) {
      int prednum = dfnum(g.GetEdgeSrc(ei->Current()));
      if (is_backedge(prednum, i)) {
        backPreds(i).push_back(prednum); 
      } else {
        nonBackPreds(i).push_back(prednum); 
      }
    } // for preds
    delete ei;
  } // for nodes
}


//
// In bottom-up traversal of depth-first spanning tree, 
// determine nested strongly connected regions and flag irreducible regions.
//                                                    phh, 4 Mar 91
//
void TarjanIntervals::GetTarjans()
{
  int w;                // DFS number of current vertex
  int firstP, firstQ;   // set and worklist

  //
  // Following loop should skip root (prenumbered as 0)
  //
  for (w = n - 1; w != ROOT_DFNUM; w--) // loop c
    //
    // skip any nodes freed or not reachable
    //
    if (w != INVALID_DFNUM && g.IsValid(vertex(w))) {
      firstP = firstQ = INVALID_DFNUM;
      //
      // Add sources of cycle arcs to P -- and to worklist Q
      //
      std::list<int>::iterator prednum; 
      for (prednum = backPreds(w).begin(); prednum != backPreds(w).end();
           prednum++) { // loop d
        int u,v;                        // vertex names
        v = *prednum;
        // ignore predecessors not reachable
        if (v != INVALID_DFNUM)
        {
          if (v == w) {
            //
            // Don't add w to its own P and Q sets
            //
            isCyclic(w) = true;
          } else {
            //
            // Add FIND(v) to P and Q
            //
            u = FIND(v);
            if (!inP(u)) {
              nextP(u) = nextQ(u) = firstP;
              firstP   = firstQ   = u;
              inP(u)   = true;
            } // if (!inP(u))
          } // if (v == w)
        } // if 
      } // for preds

      //
      // P nonempty -> w is header of a loop
      //
      if (firstP != INVALID_DFNUM)
        isCyclic(w) = true;

      while (firstQ != INVALID_DFNUM) {
        int x, y, yy;           // DFS nums of vertices

        x = firstQ;
        firstQ = nextQ(x);      // remove x from worklist

        //
        // Now look at non-cycle arcs
        //
        std::list<int>::iterator prednum;
        for (prednum = nonBackPreds(x).begin();
             prednum != nonBackPreds(x).end();
             prednum++) { // loop d
          y = *prednum;

          //
          // ignore predecessors not reachable
          //
          if (y != INVALID_DFNUM)
          {
            //
            // Add FIND(y) to P and Q
            //
            yy = FIND(y);

            if (is_backedge(yy, w)) {
              if ((!inP(yy)) & (yy != w)) {
                nextP(yy) = firstP;
                nextQ(yy) = firstQ;
                firstP = firstQ = yy;
                inP(yy) = true;
              }
              //
              // Slight change to published alg'm...
              // moved setting of header (HIGHPT)
              // from here to after the union.
              //
            } else {
              //
              // Irreducible region!
              //
              reducible(w) = false;
#if 1
              // FIXME: The DSystem version of the code did not have the
              // following line.  However, this line IS in the 1997 article,
              // and I believe it is necessary (jle, 03-02-2002).
              nonBackPreds(w).push_back(yy);
#endif
            }
          }
        }
      }
      //
      // now P = P(w) as in Tarjan's paper
      //
      while (firstP != INVALID_DFNUM) {
        //
        // First line is a change to published algorithm;
        // Want sources of cycle edges to have header w
        // and w itself not to have header w.
        //
        if ((header(firstP) == ROOT_DFNUM) & (firstP != w))
          header(firstP) = w;
        UNION(firstP, w, w);
        inP(firstP) = false;
        firstP = nextP(firstP);
      } // while
    } // if
}


//
// Traverse df spanning tree, building tree of Tarjan intervals
//
void TarjanIntervals::Build()
{
  int w;                // DFS number of current vertex
  int outer;            // DFS number header of surrounding loop

  TARJ_nodeid(ROOT_DFNUM) = g.GetRootNode();
  //
  // Let the root of the tree be the root of the instance...
  // Following loop can skip the root (prenumbered 0)
  //
  for (w = ROOT_DFNUM + 1; w < n; w++) {
    RIFGNodeId wnode = vertex(w);
    //
    // skip any nodes not in current instance g
    //
    if (wnode != RIFG_NIL) {
      outer = header(w);
      TARJ_outer(w) = outer;
      TARJ_nodeid(w) = wnode;
      //
      // tarj[w].inners = INVALID_DFNUM;  % done in InitArrays
      //
      if (isCyclic(w)) {
        //
        // Level is deeper than outer if this is a loop
        //
        if (reducible(w)) {
          TARJ_type(w) = RI_TARJ_INTERVAL;
          TARJ_level(w) = TARJ_level(outer) + 1;
        } else {
          TARJ_type(w) = RI_TARJ_IRREDUCIBLE;
          TARJ_level(w) = TARJ_level(outer);
        }
      } else {
        //
        // tarj[w].type  = RI_TARJ_ACYCLIC;     % done in InitArrays
        //
        TARJ_level(w) = TARJ_level(outer);
      }
      TARJ_next(w) = TARJ_inners(outer);
      TARJ_inners(outer) = w;
    }
  }
  n = 0;
  Prenumber(ROOT_DFNUM);
}


void TarjanIntervals::Prenumber(int v)
{
  int inner;

  tarj[v].prenum = ++n;
  last_id = TARJ_nodeid(v);
    
  for (inner = TARJ_inners(v); inner != INVALID_DFNUM; inner = TARJ_next(inner)) {
    Prenumber(inner);
  }

  /* tarj[v].last = n;  // 3/18/93 RvH: switch to RIFGNodeId last_id */
  tarj[v].last_id = last_id;
  tarj[v].last = dfnum(last_id);
}


void TarjanIntervals::Renumber()
{
  tarj = tarj;
  n = 0;
  Prenumber(ROOT_DFNUM);
}


//  Free all space used in computation of 
//  Tarjan interval tree (but not tree itself)
void TarjanIntervals::FreeWork()
{
  delete [] wk;
  wk = 0;

  delete uf;
  uf = 0;
}


static int loopIndex;
void TarjanIntervals::ComputeIntervalIndexSubTree(int node, int value)
{
  int kid, valKid;
    
  TARJ_loopIndex(node) = value;
  if (TARJ_inners(node) != INVALID_DFNUM)
    valKid = ++loopIndex;

  for (kid = TARJ_inners(node); kid != INVALID_DFNUM; kid = TARJ_next(kid))
    ComputeIntervalIndexSubTree(kid, valKid);
}


void TarjanIntervals::ComputeIntervalIndex()
{
  loopIndex = 0;
  ComputeIntervalIndexSubTree(ROOT_DFNUM, loopIndex);
}

//
// Feb. 2002 (jle): Got rid of all the silly concatenating of blank
// strings. The %*s format specifier is far simpler for creating indentations.
//
void TarjanIntervals::DumpSubTree(int node, int indent)
{
  static const char *IntervalType[] = {"NOTHING", "Acyclic",
                                  "Interval", "Irreducible"};
  //
  // Indent by three
  //
  if (indent < 72)
    indent += 3;

  printf("%*s%d(%d,%s)::%d\n", indent, " ",
         TARJ_nodeid(node), TARJ_level(node),
         IntervalType[(int) (TARJ_type(node))], TARJ_loopIndex(node));

  for (int kid = TARJ_inners(node); kid != INVALID_DFNUM; kid = TARJ_next(kid))
    DumpSubTree(kid, indent);

  //
  // Unindent
  //
  if (indent >= 3)
    indent -= 3;
}


void TarjanIntervals::Dump()
{
  printf("Tarjan interval tree: <node id>(level,type)::IntervalIndex\n");
  DumpSubTree(ROOT_DFNUM, 0 /* Indent */);
}


int TarjanIntervals::FIND(int v)
{
  return uf->Find(v);
}


void TarjanIntervals::UNION(int i, int j, int k)
{
  uf->Union(i,j,k);
}


//
// Return number of loops exited in traversing g edge from src to sink
// (0 is normal).
//
int TarjanIntervals::tarj_exits(RIFGNodeId src, RIFGNodeId sink)
{
  RIFGNodeId lca = LCA(src, sink);
  if (lca == RIFG_NIL)
    return 0;

  int dfn_src = dfnum(src);
  int dfn_lca = dfnum(lca);
  return std::max(0, TARJ_level(dfn_src) - TARJ_level(dfn_lca));
}


//
// Return outermost loop exited in traversing g edge from src to sink
// (RIFG_NIL if no loops exited).
//
RIFGNodeId TarjanIntervals::tarj_loop_exited(RIFGNodeId src, RIFGNodeId sink)
{
  RIFGNodeId lca = LCA(src, sink);
  if (lca == RIFG_NIL)
    return RIFG_NIL;

  if (lca == src)
    return RIFG_NIL;

  int dfn_src = dfnum(src);
  while (!Contains(TARJ_nodeid(TARJ_outer(dfn_src)), sink))
    dfn_src = TARJ_outer(dfn_src);
    
  if (TARJ_type(dfn_src) == RI_TARJ_INTERVAL
      || TARJ_type(dfn_src) == RI_TARJ_IRREDUCIBLE)
    return TARJ_nodeid(dfn_src);

  return RIFG_NIL;
}


//
// Return type of g edge from src to sink, one of
//   RI_TARJ_NORMAL
//   RI_TARJ_LOOP_ENTRY
//   RI_TARJ_IRRED_ENTRY
//   RI_TARJ_ITERATE (back edge)
//
RITarjEdgeType TarjanIntervals::tarj_edge_type(RIFGNodeId src,
                            RIFGNodeId sink)
{
  RIFGNodeId anc = LCA(src, sink);
  int dfn_sink = dfnum(sink);
  int dfn_anc = dfnum(anc);

  if (dfn_anc == dfn_sink) {
    return RI_TARJ_ITERATE;
  } else if (TARJ_outer(dfn_sink) != INVALID_DFNUM
           && (TARJ_type(TARJ_outer(dfn_sink)) == RI_TARJ_IRREDUCIBLE)
           && (dfn_anc != TARJ_outer(dfn_sink))) {
    //
    // Entering irreducible region not through the "header"
    //
    return RI_TARJ_IRRED_ENTRY;
  } else {
    switch (TARJ_type(dfn_sink)) {
    case RI_TARJ_INTERVAL:
      return RI_TARJ_LOOP_ENTRY;
    case RI_TARJ_IRREDUCIBLE:
      //
      // Entering irreducible region through the "header"
      //
      return RI_TARJ_IRRED_ENTRY;
    case RI_TARJ_ACYCLIC:
    case RI_TARJ_NOTHING:
    default:
      return RI_TARJ_NORMAL;
    }
  }
}


//
// LCA = least common ancestor
//
RIFGNodeId TarjanIntervals::LCA(RIFGNodeId a, RIFGNodeId b)
{
  if ((a == RIFG_NIL) || (b == RIFG_NIL))
    return RIFG_NIL;

  if (Contains(a,b))
    return a;

  int dfn_b = dfnum(b);
  while ((dfn_b != INVALID_DFNUM) && !Contains(b,a)) {
    dfn_b = TARJ_outer(dfn_b);
    b = TARJ_nodeid(dfn_b);
  }

  return b;
}


//
// Return true if the CFG edge passed in is an backedge.
//
//   Precondition: Tarjan tree must be built already.
//
int TarjanIntervals::IsBackEdge(RIFGEdgeId edgeId)
{
  RIFGNodeId src, dest;
  src = g.GetEdgeSrc(edgeId);
  dest = g.GetEdgeSink(edgeId);
  return is_backedge(dfnum(src), dfnum(dest));
} 


RIFGNodeId
TarjanIntervals::TarjOuter(RIFGNodeId id)
{
  int dfn_outer = TARJ_outer(dfnum(id));
  return dfn_outer == INVALID_DFNUM ? RIFG_NIL : TARJ_nodeid(dfn_outer);
}


RIFGNodeId TarjanIntervals::TarjInners(RIFGNodeId id)
{
  int dfn_inners = TARJ_inners(dfnum(id));
  return dfn_inners == INVALID_DFNUM ? RIFG_NIL : TARJ_nodeid(dfn_inners);
}


RIFGNodeId TarjanIntervals::TarjNext(RIFGNodeId id)
{
  int dfn_next = TARJ_next(dfnum(id));
  return dfn_next == INVALID_DFNUM ? RIFG_NIL : TARJ_nodeid(dfn_next);
}


RIFGNodeId TarjanIntervals::TarjInnersLast(RIFGNodeId id)
{
  int dfn_last_id = TARJ_last_id(dfnum(id));
  return TARJ_nodeid(dfn_last_id);
}


int TarjanIntervals::IsHeader(RIFGNodeId id)
{
  return (int)(TarjInners(dfnum(id)) != INVALID_DFNUM);
}


int TarjanIntervals::IsFirst(RIFGNodeId id)
{
  int dfn_id = dfnum(id);

  if (TarjOuter(dfn_id) == INVALID_DFNUM)
    return true;

  return (int)(TarjInners(TarjOuter(dfn_id)) == dfn_id);
}


int TarjanIntervals::IsLast(RIFGNodeId id)
{
  return (TarjNext(dfnum(id)) == INVALID_DFNUM);
}


int TarjanIntervals::Level(RIFGNodeId id)
{
  return (int) TARJ_level(dfnum(id));
}


RITarjType TarjanIntervals::IntervalType(RIFGNodeId id)
{
  return TARJ_type(dfnum(id));
}


int TarjanIntervals::Contains(RIFGNodeId a, RIFGNodeId b)
{
  return (bool)(TARJ_contains(dfnum(a), dfnum(b)));
}


int TarjanIntervals::LoopIndex(RIFGNodeId id)
{
  return TARJ_loopIndex(dfnum(id));
}

