// $Id: SSA.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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
// standard headers
#include "SSA.h"

#include <iostream>
using std::ostream;
using std::endl;
using std::cout;

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** SSA constructor computes the dominator tree, and dominance frontiers from it, for the given CFG.  Using the
    dominance frontiers, it then inserts phi functions using the following algorithm (from the book "Engineering a
    Compiler", by Keith D. Cooper and Linda Torczon, chapter "Data-flow Analysis").  The set NonLocals is the set of
    all upwardly exposed uses in each node of the CFG and is computed using the CFG method "compute_uses_sets".  The
    map Blocks maps a variable name i to the set of CFG nodes (basic blocks) where it is defined.

    <pre>
    for each name i in NonLocals
        WorkList <-- Blocks(i)
        for each block b in WorkList
            for each block d in Dominance_Frontier(b)
                insert a phi function for i in d
                WorkList <-- WorkList + d
    </pre>
*/
SSA::SSA (CFG& _cfg)
{
  cfg = &_cfg;
  DomTree dt(*cfg);
  cfg->compute_uses_sets();
  dt.compute_dominance_frontiers();
  cout << "====================" << endl << "Dominator tree " << endl << "--------------------" << endl;
  dt.dump(cout);
  cout << "====================" << endl;

  // insert Phi functions where needed
  CFG::NonLocalsIterator i(*cfg);
  std::set<CFG::Node*> work_list;
  std::set<CFG::Node*> blks_with_phi;
  while ((bool)i) {
    work_list.clear();
    blks_with_phi.clear();
    SymHandle var_name = cfg->GetIRInterface().GetSymHandle((LeafHandle)i);
    CFG::DefBlocksIterator block(*cfg, var_name);
    while ((bool)block) {
      work_list.insert((CFG::Node*)block);
      ++block;
    }
    std::set<CFG::Node*>::iterator b = work_list.begin();
    while (b != work_list.end()) {
      DomTree::DomFrontIterator d(dt.domtree_node(*b));
      while ((bool)d) {
        CFG::Node* blk = dynamic_cast<CFG::Node*>((DGraph::Node*)d);
        if (blks_with_phi.find(blk) == blks_with_phi.end()) {
          Phi* p = new Phi(var_name, cfg);
          phi_node_sets[blk].insert(p);
          cout << "inserted Phi node ";
          p->dump(cout);
          cout << " in node ";
          blk->dump(cout);
          cout << endl;
          if (work_list.find(blk) == work_list.end())
            work_list.insert(blk);
        }
        ++d;
      }
      ++b;
    }
    ++i;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
SSA::dump (ostream& os)
{
#if 0 // FIXME
  os << "===== SSA dump =====" << endl << "    " << cfg->subprog_name().name() << endl << "--------------------"
#else
  os << "===== SSA dump =====" << endl << "    " << cfg->GetIRInterface().GetSymNameFromSymHandle(cfg->subprog_name()) << endl << "--------------------"
#endif
     << endl;
  // dump each individual CFG node and the phi nodes associated with each node
  CFG::NodesIterator cfg_nodes_iter(*cfg);
  while ((bool)cfg_nodes_iter) {
    CFG::Node* n = dynamic_cast<CFG::Node*>((DGraph::Node*)cfg_nodes_iter);
    n->longdump(cfg, os);
    PhiNodesIterator phi_iter(*this, n);
    if ((bool)phi_iter) {
      os << "Phi Nodes:" << endl;
      while ((bool)phi_iter) {
        os << "    ";
        ((Phi*)phi_iter)->dump(os);
        os << endl;
        ++phi_iter;
      }
    }
    os << endl;
    ++cfg_nodes_iter;
  }
  os << endl;
}
//--------------------------------------------------------------------------------------------------------------------
