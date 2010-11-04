/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/

#ifndef __RoseFile__
#define __RoseFile__

#include "RoseBin_abstract.h"
#include <stdio.h>
#include <iostream>
// #include "rose.h"
#include "RoseBin_file.h"

class RoseFile : public RoseBin_abstract {
 private:
  // the DB connection
  RoseBin_FILE* idaDB; 
  //RoseBin_unparse* unparser;
  //SgAsmNode* globalNode;
  //std::list<std::string> functionNames;

  void loadAST(std::string filename);
  void saveAST(std::string filename);



 public:

// DQ (10/20/2010): Moved to source file to support compilation of language only mode which excludes binary analysis support.
   RoseFile(std::string file);
  
// DQ (10/20/2010): Moved to source file to support compilation of language only mode which excludes binary analysis support.
  ~RoseFile();

  // allow filtering of functions
  void setFunctionFilter(std::list<std::string> functionName);

  void visit(SgNode* node);

  // query the DB to retrieve all data
  SgAsmNode* retrieve_DB();

  // unparse the AST to assembly
  void unparse(char* fileName);

  void test();
};

#endif

