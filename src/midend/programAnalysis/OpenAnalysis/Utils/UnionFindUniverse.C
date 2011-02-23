// $Id: UnionFindUniverse.C,v 1.1 2004/07/07 10:26:35 dquinlan Exp $
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


// This code was taken from the HPCVIEW system, which took it from
// the D system.
 
//***********************************************************************************************
// UNION-FIND algorithm from Aho, Hopcroft & Ullman,
// The Design and Analysis of Computer Algorithms, Addison Wesley 1974
//***********************************************************************************************

#include "UnionFindUniverse.h"


#define UF_NIL -1


//***********************************************************************************************
// internal class UnionFindElement
//***********************************************************************************************

class UnionFindElement {
public:
  UnionFindElement() { };

  int &Count() { return count; };
  int &Root() { return root; };
  int &Parent() { return parent; }
  int &Name() { return name; }
  void Init(int i); 
private:
  int parent; 
  int name;
  int count;
  int root;
};


void UnionFindElement::Init(int i) 
{
  name = root = i; 
  parent = UF_NIL;
  count = 1;
}
        

//***********************************************************************************************
// class UnionFindUniverse interface operations
//***********************************************************************************************


UnionFindUniverse::UnionFindUniverse(unsigned int highWaterMark)
{
  e = new UnionFindElement[highWaterMark];

  for (unsigned int i = 0; i < highWaterMark; i++) {
    e[i].Init(i);
  }
}

UnionFindUniverse::~UnionFindUniverse()
{
  if (e)
    delete [] e;
}


int UnionFindUniverse::Find(int v)
{
  if (Parent(v) == UF_NIL)
    return Name(v);

  Parent(v) = do_FIND(Parent(v));

  return Name(Parent(v));
}


void UnionFindUniverse::Union(int i, int j, int k)
{
  if ((i == j) && (j == k))
    return;

  int large, small;
  if (Count(Root(i)) > Count(Root(j))) {
    large = Root(i);
    small = Root(j);
  } else {
    large = Root(j);
    small = Root(i);
  }

  Parent(small) = large;
  Count(large) += Count(small);
  Name(large) = k;
  Root(k) = large; 
}



//***********************************************************************************************
// class UnionFindUniverse private operations
//***********************************************************************************************

int UnionFindUniverse::do_FIND(int v)
{
  if (Parent(v) == UF_NIL)
    return v;

  Parent(v) = do_FIND(Parent(v));

  return Parent(v);
}

int &UnionFindUniverse::Count(int i) 
{
  return e[i].Count();
}

int &UnionFindUniverse::Root(int i) 
{
  return e[i].Root();
}
int &UnionFindUniverse::Parent(int i) 
{
  return e[i].Parent();
}

int &UnionFindUniverse::Name(int i) 
{
  return e[i].Name();
}

