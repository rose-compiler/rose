#ifndef BUFFEROVERFLOW_R_H
#define BUFFEROVERFLOW_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"
#include "RoseBin_DataFlowAbstract.h"
#include "RoseBin_DefUseAnalysis.h"
//#include "RoseBin_VariableAnalysis.h"

class BufferOverflow : public BinAnalyses,  RoseBin_DataFlowAbstract {
 public:
 BufferOverflow(GraphAlgorithms* algo):RoseBin_DataFlowAbstract(algo)
  {testFlag=false;debug=false;};
  virtual ~BufferOverflow(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();

  bool run(std::string& name, SgGraphNode* node,
           SgGraphNode* previous);

  bool twoFiles() {return false;}
  std::map<SgNode*,std::string> getResult(){return result;}
 private:
  std::map<SgNode*,std::string> result;
  bool debug;
  VirtualBinCFG::AuxiliaryInformation* info;
  void init(RoseBin_Graph* vg) {
    vizzGraph = vg;
  }
  bool runEdge(SgGraphNode* node, SgGraphNode* next) {
    return false;
  }

  BinQGUI *instance;
};



#endif
