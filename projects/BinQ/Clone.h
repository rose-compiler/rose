#ifndef CLONE_R_H
#define CLONE_R_H
#include "rose.h"


#include <iostream>
#include <list>
#include "BinAnalyses.h"

class BinQGUI;

class DiffAlgo : public BinAnalyses {
 public:
  DiffAlgo(){};
  virtual ~DiffAlgo(){};
  void run();
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return true;}

};

//Find matching instructions between insnsA and the global instructions for file A. 
//Color the table row for the instruction in file A that matches the one in file A. Repeat for insns A and file B.
void colorTable(BinQGUI* instance, const std::vector<std::pair<int,int> >& addInstr,  const std::vector<std::pair<int,int> >&  minusInst,
       LCS::vector_start_at_one<SgNode*>& insnA, LCS::vector_start_at_one<SgNode*>& insnB 
 );


#endif
