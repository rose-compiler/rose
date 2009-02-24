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
  SgAsmNode* globalNode;
  char* filename;
  std::ofstream myfile;

 public:

  RoseBin_printInstructions(){};

  // initialize with the globalNode and the filename for output
  void initPrint(SgAsmNode* root, char* fileName);

  // unparse the AST to assembly
  void unparsePrint();

  /****************************************************
   * unparse binary instruction
   ****************************************************/
  virtual void visit(SgNode* n);

  /****************************************************
   * close the file
   ****************************************************/
  void closeFile();

};

#endif


