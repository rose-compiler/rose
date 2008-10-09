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

  SgDirectedGraphNode* nodeBefore;


  void addDefElement(SgDirectedGraphNode* sgNode,
		     std::pair<X86RegisterClass, int>  initName,
		     SgDirectedGraphNode* defNode);

  void addUseElement(SgDirectedGraphNode* sgNode,
		     std::pair<X86RegisterClass, int>  initName,
		     SgDirectedGraphNode* defNode);
 
  void addAnyElement(tabletype* tabl, SgDirectedGraphNode* sgNode,
		     std::pair<X86RegisterClass, int>  initName,
		     SgDirectedGraphNode* defNode);





  bool searchDefMap(SgDirectedGraphNode* node);
  bool searchMap(const tabletype* ltable, SgDirectedGraphNode* node);


  //void handleDefCopy(SgDirectedGraphNode* sgNode, 
  //		     int nrOfInEdges,
  //	     SgDirectedGraphNode* sgNodeBefore);


  void mapDefUnion(SgDirectedGraphNode* before, SgDirectedGraphNode* other, SgDirectedGraphNode* sgNode);
  void mapUseUnion(SgDirectedGraphNode* before, SgDirectedGraphNode* other, SgDirectedGraphNode* sgNode);
  void mapAnyUnion(tabletype* tabl, SgDirectedGraphNode* before, SgDirectedGraphNode* other, SgDirectedGraphNode* sgNode); 

  bool checkElementsForEquality(const multitype* t1, const multitype* t2);
  void getOtherInNode(std::vector<SgDirectedGraphNode*>& vec,
		      SgDirectedGraphNode* cfgNode, SgDirectedGraphNode* oneNode);

  void printDefMap();
  void printUseMap();
  void printAnyMap(tabletype* tabl);
  void printMultiMap(const multitype* multi);

  bool searchMulti(const multitype* multi, std::pair<X86RegisterClass, int>  initName);
  bool searchMulti(const multitype* multi, std::pair<X86RegisterClass, int>  initName,
				    SgDirectedGraphNode* val);

  void replaceElement(SgDirectedGraphNode* sgNode,
		      std::pair<X86RegisterClass, int>  initName);
  void clearRegisters();

  void handleCopy(bool def,SgDirectedGraphNode* sgNode, SgDirectedGraphNode* sgNodeBefore);


 public:

  RoseBin_DefUseAnalysis() {
  }
  ~RoseBin_DefUseAnalysis() {}

  bool run(std::string& name, SgDirectedGraphNode* node,SgDirectedGraphNode* nodeBefore);
	  

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
 
  std::string getElementsAsStringForNode(bool def,SgDirectedGraphNode* node);

  bool runEdge( SgDirectedGraphNode* node, SgDirectedGraphNode* next);



};

#endif

