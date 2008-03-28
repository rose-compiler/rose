#ifndef __RoseBin_abstract__
#define __RoseBin_abstract__



#include <stdio.h>
#include <iostream>


class RoseBin_abstract : public AstSimpleProcessing {
 protected: 
  RoseBin_unparse* unparser;
  SgAsmNode* globalNode;
  std::list<std::string> functionNames;

  int trav_inst;
  int trav_blocks;
  int trav_funcs;
  int nodes;

  int num_func;
  int num_inst;


  void loadAST(std::string filename);
  void saveAST(std::string filename);



 public:

  RoseBin_abstract() {
    unparser = new RoseBin_unparse();
    RoseBin_support::setUnparseVisitor(unparser->getVisitor());
    globalNode = 0;
    functionNames.clear();
    num_inst=0;
    num_func=0;
    RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::none;
  }
  
  ~RoseBin_abstract() {
    delete unparser;

    unparser = NULL;

    if (globalNode)
      delete globalNode;
  }

  int getNumberOfFunctions() {return (num_func-1);}
  int getNumberOfInstructions() {return (num_inst-1);}

};

#endif

