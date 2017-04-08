/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Aug08
 * Decription : printInstruction
 ****************************************************/

#ifndef __RoseBin_printInstruction_
#define __RoseBin_printInstruction_

#include <stdio.h>
#include <iostream>

#include "RoseBin_support.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <bitset>

class RoseBin_printInstructions : public AstSimpleProcessing {
 private:
#if 0 // [Robb P Matzke 2017-03-27]
  SgAsmNode* globalNode;
  char* filename;
#endif
  std::ofstream myfile;

 public:

  RoseBin_printInstructions(){};

  // initialize with the globalNode and the filename for output
  void initPrint(SgAsmNode* root, char* fileName) ROSE_DEPRECATED("no longer supported");

  // unparse the AST to assembly
  void unparsePrint() ROSE_DEPRECATED("no longer supported");

  /****************************************************
   * unparse binary instruction
   ****************************************************/
  virtual void visit(SgNode* n) ROSE_DEPRECATED("no longer supported");

  /****************************************************
   * close the file
   ****************************************************/
  void closeFile() ROSE_DEPRECATED("no longer supported");

};

#endif


