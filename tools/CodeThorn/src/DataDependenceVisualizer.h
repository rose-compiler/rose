#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
#include <string>

#include "AstUtility.h"
#include "CFAnalysis.h"
#include "TopologicalSort.h"

namespace CodeThorn {
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, std::string useDefAttributeName);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  void includeFlowGraphEdges(Flow* flow) { _flow=flow; }
  void generateDotFunctionClusters(SgNode* root, CFAnalysis* cfanalyzer, std::string fileName, TopologicalSort* topSort);
  std::string nodeSourceCode(Label lab);
  bool _showSourceCode;
  void setFunctionLabelSetSets(LabelSetSet);
  void setDotGraphName(std::string);
  std::string getDotGraphName();
 private:
  void generateDot(SgNode* root, std::string fileName);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  std::string  _useDefAttributeName;
  Flow* _flow;
  LabelSetSet _functionLabelSetSets;
  std::string _dotGraphName;
  //std::string _dotFunctionClusters;
};

}

#endif
