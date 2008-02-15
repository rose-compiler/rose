// $Id: Phi.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

#ifndef PHI_H
#define PHI_H

//--------------------------------------------------------------------------------------------------------------------
// STL headers
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/CFG/CFG.h>
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
class Phi {
public:
  Phi (const SymHandle& var_name, CFG* _cfg) { sym = var_name; cfg = _cfg; }
  ~Phi () {}
  void dump (ostream&);
  void add_arg (CFG::Node* c_n, LeafHandle a_n) { args[c_n] = a_n; }
  LeafHandle arg (CFG::Node* n) { return args[n]; }
  int num_args () { return args.size(); }

  //------------------------------------------------------------------------------------------------------------------
  class ArgIterator : public Iterator {
  public:
    ArgIterator (Phi& p) { center = &p; iter = center->args.begin(); }
    void operator++ () { ++iter; }
    operator bool () { return (iter != center->args.end()); }
    operator CFG::Node* () { return (*iter).first; }
    operator LeafHandle () { return (*iter).second; }
  private:
    std::map<CFG::Node*, LeafHandle>::iterator iter;
    Phi* center;
  };
  friend class ArgIterator;
  //------------------------------------------------------------------------------------------------------------------

private:
  std::map<CFG::Node*, LeafHandle> args;
  SymHandle sym;
  CFG* cfg;
};
//--------------------------------------------------------------------------------------------------------------------

#endif
