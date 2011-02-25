// $Id: OARIFG.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

#ifdef NO_STD_CHEADERS
# include <assert.h>
#else
# include <cassert>
#endif

#include "RIFG.h"
#include "OARIFG.h"
#include "CFG.h"

//===============================================================================================
// Representation Independent Flowgraph Edge Iterator
//===============================================================================================
// FIXME: This code could be made more efficient. 

class OARIFGEdgeIterator : public RIFGEdgeIterator {
public:
  OARIFGEdgeIterator (RIFG &_fg, CFG::Node *nip,
                      RIFG::EdgeDirection _dir):fg(_fg),dir(_dir),
                      oa_out_iter(nip),oa_in_iter(nip) { }

  RIFGEdgeId Current() {
    CFG::Edge *ep = 0;
    if (dir == RIFG::ED_INCOMING) {
      if ((bool)oa_in_iter) {
        ep = dynamic_cast<CFG::Edge *>((DGraph::Edge *)(oa_in_iter));
        return ((OARIFG &)fg).edge_to_id_map[ep];
      } else { 
        return RIFG_NIL;
      }
    } else {
      if ((bool)oa_out_iter) {
        ep = dynamic_cast<CFG::Edge *>((DGraph::Edge *)(oa_out_iter));
        return ((OARIFG &)fg).edge_to_id_map[ep];
      } else {
        return RIFG_NIL;
      }
    }
  }     

  RIFGEdgeId operator++(int) {
    CFG::Edge *old;
    if (dir == RIFG::ED_INCOMING) {
      if ((bool)oa_in_iter) {
        old = dynamic_cast<CFG::Edge *>((DGraph::Edge *)oa_in_iter);
        ++oa_in_iter;
        return ((OARIFG &)fg).edge_to_id_map[old];
      } else
        return RIFG_NIL;
    } else {
      if ((bool)oa_out_iter) {
        old = dynamic_cast<CFG::Edge *>((DGraph::Edge *)oa_out_iter);
        ++oa_out_iter;
        return ((OARIFG &)fg).edge_to_id_map[old];
      } else
        return RIFG_NIL;
    }
  }
  void Reset() { assert(0); };
  virtual ~OARIFGEdgeIterator() { };
private:
  RIFG &fg;
  RIFG::EdgeDirection dir;
  CFG::OutgoingEdgesIterator oa_out_iter;
  CFG::IncomingEdgesIterator oa_in_iter;
};


//===============================================================================================
// Representation Independent Flowgraph Node Iterator
//===============================================================================================
class OARIFGNodeIterator : public RIFGNodeIterator {
public:
  OARIFGNodeIterator (RIFG &fg, RIFG::ForwardBackward _fb) :
                      oa_iter(((OARIFG &)fg).cfg, _fb == RIFG::FORWARD ?
                      CFG::BiDirNodesIterator::Forward :
                      CFG::BiDirNodesIterator::Reverse) { 
    fb = _fb;
  }

  RIFGNodeId Current() {
    if ((bool)(oa_iter)) {
      CFG::Node *nnn = dynamic_cast<CFG::Node*>((BaseGraph::Node*)(oa_iter));
      return nnn->getID();
    } else
      return RIFG_NIL;
  }

  RIFGNodeId operator++(int) {
    CFG::Node *old;
    if (fb == RIFG::FORWARD) {
      if ((bool)(oa_iter)) {
        old = dynamic_cast<CFG::Node*>((BaseGraph::Node*)(oa_iter));
        ++(oa_iter);
      } else
        old = 0;
    } else {
      if ((bool)(oa_iter)) {
        old = dynamic_cast<CFG::Node*>((BaseGraph::Node*)(oa_iter));
        --(oa_iter);
      } else
        old = 0;
    }
    return old ? (int)old->getID() : RIFG_NIL; // FIXME
  }

  void Reset() { assert(0); }
  virtual ~OARIFGNodeIterator() {};
private:
  RIFG::ForwardBackward fb;     
  CFG::BiDirNodesIterator oa_iter;
};


OARIFG::OARIFG(CFG &_cfg):cfg(_cfg)
{
  // Initialize the nodeid to node map.
  CFG::NodesIterator ni(cfg);
  while ((bool)ni) {
    CFG::Node *nnn = dynamic_cast<CFG::Node *>((DGraph::Node *)ni);
    nodeid_to_node_map[nnn->getID()] = nnn;
    ++ni;
  }

  // Create edges id's (move to CFG?)
  // and Initialize the edgeid to edge map.
  CFG::EdgesIterator ei(cfg);
  int num = 1;
  while ((bool)ei) {
    CFG::Edge *eee = dynamic_cast<CFG::Edge *>((DGraph::Edge *)ei);
    edge_to_id_map[eee] = num;
    edgeid_to_edge_map[num] = eee;
    ++num;
    ++ei;
  }
}


OARIFG::~OARIFG()
{
  nodeid_to_node_map.clear();
  edge_to_id_map.clear();
  edgeid_to_edge_map.clear();
}


unsigned int OARIFG::HighWaterMarkNodeId()  // largest node id in the graph
{
  return cfg.num_nodes();
}

int OARIFG::IsValid(RIFGNodeId n)     // is the node id still valid, or has it been freed
{
  return 1;
}

int OARIFG::GetFanin(TarjanIntervals *tj, RIFGNodeId n)
{
  assert(0);
  return 0;
}

RIFGNodeId OARIFG::GetRootNode()
{
  return (cfg.Entry())->getID();
}

RIFGNodeId OARIFG::GetFirstNode()
{
  assert (0);
  return 0;
}

RIFGNodeId OARIFG::GetLastNode()
{
  assert (0);
  return 0;
}

RIFGNodeId OARIFG::GetNextNode(RIFGNodeId n)
{
  assert (0);
  return 0;
}

RIFGNodeId OARIFG::GetPrevNode(RIFGNodeId n)
{
  assert (0);
  return 0;
}

RIFGNodeId OARIFG::GetEdgeSrc(RIFGEdgeId eid)
{
  CFG::Edge *e = edgeid_to_edge_map[eid];
  DGraph::Node *n = ((DGraph::Edge *)e)->source();
  return ((CFG::Node *)n)->getID();
}

RIFGNodeId OARIFG::GetEdgeSink(RIFGEdgeId eid)
{
  CFG::Edge *e = edgeid_to_edge_map[eid];
  DGraph::Node *n = ((DGraph::Edge *)e)->sink();
  return ((CFG::Node *)n)->getID();
}

RIFGNodeId *OARIFG::GetTopologicalMap(TarjanIntervals *tj)
{
  assert (0);
  return 0;
}

RIFGNode *OARIFG::GetRIFGNode(RIFGNodeId nid)
{
  CFG::Node *n = nodeid_to_node_map[nid];
  return (RIFGNode *)n;
}

RIFGEdge *OARIFG::GetRIFGEdge(RIFGEdgeId eid)
{
  CFG::Edge *e = edgeid_to_edge_map[eid];
  return (RIFGEdge *)e;
}


RIFGEdgeIterator *OARIFG::GetEdgeIterator(RIFG &fg, RIFGNodeId n,  RIFG::EdgeDirection ed)
{
  return new OARIFGEdgeIterator(fg, nodeid_to_node_map[n], ed);
}

RIFGNodeIterator *OARIFG::GetNodeIterator(RIFG &fg, RIFG::ForwardBackward fb)
{
  return new OARIFGNodeIterator(fg, fb);
}

