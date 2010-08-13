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
		     SgGraphNode* defNode);

  void addUseElement(SgGraphNode* sgNode,
                     std::pair<X86RegisterClass, int>  initName,
		     SgGraphNode* defNode);

  void addAnyElement(tabletype* tabl, SgGraphNode* sgNode,
                     std::pair<X86RegisterClass, int>  initName,
		     SgGraphNode* defNode);





  bool searchDefMap(SgGraphNode* node);
  bool searchMap(const tabletype* ltable, SgGraphNode* node);


  //void handleDefCopy(SgGraphNode* sgNode,
  //		     int nrOfInEdges,
  //	     SgGraphNode* sgNodeBefore);


  void mapDefUnion(SgGraphNode* before, SgGraphNode* other, SgGraphNode* sgNode);
  void mapUseUnion(SgGraphNode* before, SgGraphNode* other, SgGraphNode* sgNode);
  void mapAnyUnion(tabletype* tabl, SgGraphNode* before, SgGraphNode* other, SgGraphNode* sgNode);

  bool checkElementsForEquality(const multitype* t1, const multitype* t2);
  void getOtherInNode(std::vector<SgGraphNode*>& vec,
		      SgGraphNode* cfgNode, SgGraphNode* oneNode);

  void printDefMap();
  void printUseMap();
  void printAnyMap(tabletype* tabl);
  void printMultiMap(const multitype* multi);

  bool searchMulti(const multitype* multi, std::pair<X86RegisterClass, int>  initName);
  bool searchMulti(const multitype* multi, std::pair<X86RegisterClass, int>  initName,
				    SgGraphNode* val);

  void replaceElement(SgGraphNode* sgNode,
		      std::pair<X86RegisterClass, int>  initName);
  void clearRegisters();

  void handleCopy(bool def,SgGraphNode* sgNode, SgGraphNode* sgNodeBefore);


 public:

 RoseBin_DefUseAnalysis(GraphAlgorithms* algo):RoseBin_DataFlowAbstract(algo) {
    ROSE_ASSERT(algo);
  }
  ~RoseBin_DefUseAnalysis() {}

  bool run(std::string& name, SgGraphNode* node,SgGraphNode* nodeBefore);


  int getNrOfMemoryWrites() {return nrOfMemoryWrites;}
  int getNrOfRegisterWrites() {return nrOfRegisterWrites;}

  void init(RoseBin_Graph* vg) {
    vizzGraph = vg;
    nrOfMemoryWrites=0;
    nrOfRegisterWrites=0;
    nodeBefore=NULL;

    deftable.clear();
    //    usetable.clear();
  }

  std::string getElementsAsStringForNode(bool def,SgGraphNode* node);

  bool runEdge( SgGraphNode* node, SgGraphNode* next);



};

#endif

