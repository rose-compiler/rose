#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
#include <string>

#include "RDAstAttribute.h"
#include "AnalysisAbstractionLayer.h"
#include "CFAnalysis.h"
#include "UDAstAttribute.h"

namespace CodeThorn {
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, std::string useDefAttributeName);
  VariableIdSet useVars(SgNode* expr);
  LabelSet defLabels(SgNode* expr, VariableId useVar);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  //! requires UDAstAttribute
  void generateDefUseDotGraph(SgNode* root, std::string fileName);
  //! requires UDAstAttribute
  void generateUseDefDotGraph(SgNode* root, std::string fileName);
  void includeFlowGraphEdges(Flow* flow) { _flow=flow; }
  void generateDotFunctionClusters(SgNode* root, CFAnalysis* cfanalyzer, std::string fileName, bool withDataDependencies);
  std::string nodeSourceCode(Label lab);
  bool _showSourceCode;
  void setFunctionLabelSetSets(LabelSetSet);
  void setDotGraphName(std::string);
  std::string getDotGraphName();
 private:
  enum DDVMode { DDVMODE_USEDEF, DDVMODE_DEFUSE };
  void generateDot(SgNode* root, std::string fileName);
  UDAstAttribute* getUDAstAttribute(SgNode* expr,std::string attributeName);
  bool existsUDAstAttribute(SgNode* expr,std::string attributeName);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  std::string  _useDefAttributeName;
  DDVMode _mode;
  Flow* _flow;
  LabelSetSet _functionLabelSetSets;
  std::string _dotGraphName;
  //std::string _dotFunctionClusters;
};

}

#endif
