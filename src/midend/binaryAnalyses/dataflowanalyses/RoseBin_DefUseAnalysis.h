/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep7 07
 * Decription : Data flow Analysis
 ****************************************************/

#ifndef __RoseBin_DefUseAnalysis__
#define __RoseBin_DefUseAnalysis__

#include "RoseBin_DataFlowAbstract.h"

class RoseBin_DefUseAnalysis  : public RoseBin_DataFlowAbstract {
 private:

  SgGraphNode* nodeBefore;


  void addDefElement(SgGraphNode* sgNode,
                     std::pair<X86RegisterClass, int>  initName,
                     SgGraphNode* defNode) ROSE_DEPRECATED("no longer supported");

  void addUseElement(SgGraphNode* sgNode,
                     std::pair<X86RegisterClass, int>  initName,
                     SgGraphNode* defNode) ROSE_DEPRECATED("no longer supported");

  void addAnyElement(tabletype* tabl, SgGraphNode* sgNode,
                     std::pair<X86RegisterClass, int>  initName,
                     SgGraphNode* defNode) ROSE_DEPRECATED("no longer supported");





  bool searchDefMap(SgGraphNode* node) ROSE_DEPRECATED("no longer supported");
  bool searchMap(const tabletype* ltable, SgGraphNode* node) ROSE_DEPRECATED("no longer supported");


  //void handleDefCopy(SgGraphNode* sgNode,
  //                 int nrOfInEdges,
  //         SgGraphNode* sgNodeBefore);


  void mapDefUnion(SgGraphNode* before, SgGraphNode* other, SgGraphNode* sgNode) ROSE_DEPRECATED("no longer supported");
  void mapUseUnion(SgGraphNode* before, SgGraphNode* other, SgGraphNode* sgNode) ROSE_DEPRECATED("no longer supported");
  void mapAnyUnion(tabletype* tabl, SgGraphNode* before, SgGraphNode* other, SgGraphNode* sgNode) ROSE_DEPRECATED("no longer supported");

  bool checkElementsForEquality(const multitype* t1, const multitype* t2) ROSE_DEPRECATED("no longer supported");
  void getOtherInNode(std::vector<SgGraphNode*>& vec,
                      SgGraphNode* cfgNode, SgGraphNode* oneNode) ROSE_DEPRECATED("no longer supported");

  void printDefMap() ROSE_DEPRECATED("no longer supported");
  void printUseMap() ROSE_DEPRECATED("no longer supported");
  void printAnyMap(tabletype* tabl) ROSE_DEPRECATED("no longer supported");
  void printMultiMap(const multitype* multi) ROSE_DEPRECATED("no longer supported");

  bool searchMulti(const multitype* multi, std::pair<X86RegisterClass, int>  initName) ROSE_DEPRECATED("no longer supported");
  bool searchMulti(const multitype* multi, std::pair<X86RegisterClass, int>  initName,
                                    SgGraphNode* val) ROSE_DEPRECATED("no longer supported");

  void replaceElement(SgGraphNode* sgNode,
                      std::pair<X86RegisterClass, int>  initName) ROSE_DEPRECATED("no longer supported");
  void clearRegisters() ROSE_DEPRECATED("no longer supported");

  void handleCopy(bool def,SgGraphNode* sgNode, SgGraphNode* sgNodeBefore) ROSE_DEPRECATED("no longer supported");


 public:

 RoseBin_DefUseAnalysis(GraphAlgorithms* algo):RoseBin_DataFlowAbstract(algo) {
    ROSE_ASSERT(algo);
  }
  ~RoseBin_DefUseAnalysis() {}

  bool run(std::string& name, SgGraphNode* node,SgGraphNode* nodeBefore) ROSE_DEPRECATED("no longer supported");


  int getNrOfMemoryWrites() ROSE_DEPRECATED("no longer supported") {return nrOfMemoryWrites;}
  int getNrOfRegisterWrites() ROSE_DEPRECATED("no longer supported") {return nrOfRegisterWrites;}

  void init(RoseBin_Graph* vg) ROSE_DEPRECATED("no longer supported") {
    vizzGraph = vg;
    nrOfMemoryWrites=0;
    nrOfRegisterWrites=0;
    nodeBefore=NULL;

    deftable.clear();
    //    usetable.clear();
  }

  std::string getElementsAsStringForNode(bool def,SgGraphNode* node) ROSE_DEPRECATED("no longer supported");

  bool runEdge( SgGraphNode* node, SgGraphNode* next) ROSE_DEPRECATED("no longer supported");



};

#endif

