// $Id: OARIFG.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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


#ifndef OARIFG_h
#define OAlRIFG_h


#include "RIFG.h"
#include "CFG.h" 


//===============================================================================================
// Representation Independent Flowgraph Interface
//===============================================================================================
class OARIFG : public RIFG {
private:
  CFG &cfg;
  std::map<RIFGNodeId, CFG::Node *> nodeid_to_node_map; 
  std::map<RIFGEdgeId, CFG::Edge *> edgeid_to_edge_map; 
public:
  std::map<CFG::Edge *, RIFGEdgeId> edge_to_id_map;     
  OARIFG(CFG &_cfg);
  ~OARIFG();
  friend class OARIFGNodeIterator;
  //-------------------------------------------------------------------------------------------
  // assumption: node identifiers are mostly dense, though some may have been freed
  //-------------------------------------------------------------------------------------------
  unsigned int HighWaterMarkNodeId();  // largest node id in the graph

  int IsValid(RIFGNodeId n);     // is the node id still valid, or has it been freed
  int GetFanin(TarjanIntervals *, RIFGNodeId);

  RIFGNodeId GetRootNode();
  RIFGNodeId GetFirstNode();
  RIFGNodeId GetLastNode();

  RIFGNodeId GetNextNode(RIFGNodeId n);
  RIFGNodeId GetPrevNode(RIFGNodeId n);

  RIFGNodeId GetEdgeSrc(RIFGEdgeId e);
  RIFGNodeId GetEdgeSink(RIFGEdgeId e);

  RIFGNodeId *GetTopologicalMap(TarjanIntervals *);

  RIFGNode *GetRIFGNode(RIFGNodeId n);
  RIFGEdge *GetRIFGEdge(RIFGEdgeId e);
        
  RIFGEdgeIterator *GetEdgeIterator(RIFG &fg, RIFGNodeId n, RIFG::EdgeDirection ed);
  RIFGNodeIterator *GetNodeIterator(RIFG &fg, RIFG::ForwardBackward fb);
};

#endif
