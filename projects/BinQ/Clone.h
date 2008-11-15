#ifndef CLONE_R_H
#define CLONE_R_H
#include "rose.h"


#include <iostream>
#include <list>
#include "BinAnalyses.h"

class DiffAlgo : public BinAnalyses {
 public:
  DiffAlgo(){};
  virtual ~DiffAlgo(){};
  void run();
  std::string name();
  std::string getDescription();

  void colorTable( const std::vector<std::pair<int,int> >& addInstr,  const std::vector<std::pair<int,int> >&  minusInst,
       LCS::vector_start_at_one<SgNode*>& insnA, LCS::vector_start_at_one<SgNode*>& insnB 
 );

};



#endif
