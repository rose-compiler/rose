/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

#ifndef __RoseBin_unparse_
#define __RoseBin_unparse_

#include <stdio.h>
#include <iostream>
// #include "rose.h"
// #include "RoseBin_support.h"

#include "RoseBin_unparse_visitor.h"

class RoseBin_unparse {
 private:
  SgAsmNode* globalNode;
  char* filename;
    RoseBin_unparse_visitor* visitor;

 public:

  RoseBin_unparse(){
    visitor = new RoseBin_unparse_visitor();
    RoseBin_support::setUnparseVisitor(visitor);
  };
  ~RoseBin_unparse(){
    if (globalNode)
      delete globalNode;
  }

  RoseBin_unparse_visitor* getVisitor() {return visitor;}

  // initialize with the globalNode and the filename for output
  void init(SgAsmNode* root, char* fileName);

  // unparse the AST to assembly
  void unparse();

};

#endif


