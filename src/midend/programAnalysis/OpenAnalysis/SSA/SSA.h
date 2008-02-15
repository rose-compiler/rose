// $Id: SSA.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

#ifndef SSA_H
#define SSA_H

//--------------------------------------------------------------------------------------------------------------------
// OpenAnalysis headers
#include "DomTree.h"
#include "Phi.h"
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
class SSA {
public:
  SSA (CFG& _cfg);
  ~SSA () {}
  class PhiNodesIterator;
  friend class PhiNodesIterator;

  //------------------------------------------------------------------------------------------------------------------
  class Use;
  class Def {
  public:
    virtual ~Def () {}
    virtual void dump (ostream&) = 0;
    virtual std::list<Use*>* uses_list () = 0;
  };
  //------------------------------------------------------------------------------------------------------------------
  class Use {
  public:
    virtual ~Use () {}
    virtual void dump (ostream&) = 0;
    virtual Def* def () = 0;
  };
  //------------------------------------------------------------------------------------------------------------------
  class LeafDef : public Def {
  public:
    LeafDef (LeafHandle l) : Def() { leaf = l; }
    void dump (ostream&);
    std::list<Use*>* uses_list () { return &uses; }
  private:
    LeafHandle leaf;
    std::list<Use*> uses;
  };
  //------------------------------------------------------------------------------------------------------------------
  class PhiDef : public Def {
  public:
    PhiDef (Phi* p) : Def() { phi = p; }
    void dump (ostream&);
    std::list<Use*>* uses_list () { return &uses; }
  private:
    Phi* phi;
    std::list<Use*> uses;
  };
  //------------------------------------------------------------------------------------------------------------------
  class LeafUse : public Use {
  public:
    LeafUse (LeafHandle l) : Use() { leaf = l; }
    void dump (ostream&);
    Def* def () { return definition; }
  private:
    LeafHandle leaf;
    Def* definition;
  };
  //------------------------------------------------------------------------------------------------------------------
  class PhiUse : public Use {
  public:
    PhiUse (Phi* p) : Use() { phi = p; }
    void dump (ostream&);
    Def* def () { return definition; }
  private:
    Phi* phi;
    Def* definition;
  };
  //------------------------------------------------------------------------------------------------------------------
  class PhiNodesIterator : public Iterator {
  public:
    PhiNodesIterator (SSA& s, CFG::Node* n) { phi_set = &s.phi_node_sets[n];  iter = phi_set->begin(); }
    void operator++ () { ++iter; }
    operator bool () { return (iter != phi_set->end()); }
    operator Phi* () { return *iter; }
  private:
    std::set<Phi*>::iterator iter;
    std::set<Phi*>* phi_set;
  };
  //------------------------------------------------------------------------------------------------------------------

  void dump (ostream&);

private:
  CFG* cfg;
  std::map<CFG::Node*, std::set<Phi*> > phi_node_sets;
};
//--------------------------------------------------------------------------------------------------------------------

#endif
